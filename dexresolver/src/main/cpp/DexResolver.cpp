

#include <jni.h>
#include <dlfcn.h>
#include <sys/mman.h>
#include "fake_dlfcn.h"
#include <errno.h>
#include <fcntl.h>
#include <sys/system_properties.h>
#include "runtime.h"
#ifdef __LP64__
#define libPath "lib64"
#else
#define libPath "lib"
#endif

static void *sRuntime;

static void* vm;

static jmethodID longValue;

extern "C" int getSDK() {
    static int sdk=0;
    if(sdk!=0)
        return sdk;
    char s[6];
    __system_property_get("ro.build.version.sdk",s);
    sdk=atoi(s);
    return sdk;
}


static void fixName(char* tmp, const char* src,int len){
    memcpy(tmp,src,(size_t)len);
    tmp[len]=0;
    for(int i=len-1;i!=-1;--i){
        if(tmp[i]=='/')
            tmp[i]='.';
    }
}
template <typename T>
static inline void bestCache(T** orig,int* cacheLen,int len){
    if(len>*cacheLen){
        delete [] *orig;
        len=len+(len>>1);
        *orig=new T[len];
        *cacheLen=len;
    }
}

static int retZero(){
    return 0;
}
void setCode(void* addr,const unsigned char* code,size_t len){
    uintptr_t start= reinterpret_cast<uintptr_t >(addr);
    uintptr_t end=start+ len;
    start=(start>>12)<<12;
    end=(end>>12)<<12;
    uintptr_t memLen= 4096;
    if(start!=end)memLen<<=1;
    int ret=mprotect(reinterpret_cast<void *>(start), memLen, PROT_READ | PROT_WRITE | PROT_EXEC);
    if(ret==-1){
        LOGE("Failed to write code: %s",strerror(errno));
        return;
    }
    memcpy(addr,code,len);
    ret=mprotect(reinterpret_cast<void *>(start), memLen, PROT_READ | PROT_EXEC);
    if(ret==-1){
        LOGE("Failed to reset code permission: %s",strerror(errno));
        return;
    }
}

static int compareInt(const void* lhs, const void* rhs){
    return (*static_cast<const uint *>(lhs))-(*static_cast<const uint *>(rhs));
}
template <typename T>
static jobjectArray getClassList(JNIEnv *env,  const T *const *dexFiles,int length) {
    jobjectArray ret = nullptr;
    char *tmp = new char[256];
    int cacheLen = 256;
    int cacheIndexLen=dexFiles[0]->header_->class_defs_size_;
    uint* idxes=new uint[cacheIndexLen];// for worst
    int index = 0;
    for (int k=length;k--;) {
        auto dexFile=dexFiles[k];
        int size = dexFile->header_->class_defs_size_;
        bestCache(&idxes, &cacheIndexLen, size);
        for (int i = 0; i <size; ++i ) {
            idxes[i]=dexFile->class_defs_[i].class_idx_;
        }
        qsort(idxes,size, sizeof(uint),compareInt);
        JObjectArray classes;
        for (int i = size; i--; ) {
            const char *bytes =dexFile->stringFromType(idxes[i]) + 1;
            int len = (int) strlen(bytes);
            bestCache(&tmp, &cacheLen, len);
            fixName(tmp, bytes, len - 1);
            auto&& str = JString(env, env->NewStringUTF(tmp));
            if(classes== nullptr){
                classes=JObjectArray(env,env->NewObjectArray(size,env->GetObjectClass(str),NULL));
            }
            env->SetObjectArrayElement(classes, i, str);
        }
        if(!ret) ret=env->NewObjectArray(length, env->GetObjectClass(classes), NULL);
        env->SetObjectArrayElement(ret,index,classes);
        ++index;
    }
    delete[] idxes;
    delete[] tmp;
    return ret;
}
static jobjectArray getClassList(JNIEnv *env,  const void *const *dexFiles,int length){
    return getSDK()<28?getClassList<art::DexFile>(env,(const art::DexFile*const *)dexFiles,length):
           getClassList<art::DexFile28>(env,(const art::DexFile28*const *)dexFiles,length);
}
static jobjectArray getClassList(JNIEnv *env,  const vector_base< const void *>*dexFiles){
    return getClassList(env, dexFiles->begin(), static_cast<int>(dexFiles->size()));
}
namespace DexResolver {
    void init() {
        int sdk = getSDK();
        void *handle = fake_dlopen("/system/" libPath "/libart.so", RTLD_NOW);
        sRuntime = *(void**)fake_dlsym(handle, "_ZN3art7Runtime9instance_E");
        if (sdk >= 28) {
            void *fMember = fake_dlsym(handle,
                                       "_ZN3art9hiddenapi6detail19GetMemberActionImplINS_8ArtFieldEEENS0_"
                                       "6ActionEPT_NS_20HiddenApiAccessFlags7ApiListES4_NS0_12AccessMethodE");
            void *mMember = fake_dlsym(handle,
                                       "_ZN3art9hiddenapi6detail19GetMemberActionImplINS_9ArtMethodEEENS0_"
                                       "6ActionEPT_NS_20HiddenApiAccessFlags7ApiListES4_NS0_12AccessMethodE");
            if (fMember != nullptr)
                setCode(fMember, (const unsigned char *) retZero, 16);
            if (mMember != nullptr)
                setCode(mMember, (const unsigned char *) retZero, 16);
        }
        fake_dlclose(handle);
    }

    template <typename Runtime>
    vector_base<const void *> * getBootDexFiles(){
        Runtime *runtime = static_cast<Runtime *>(sRuntime);
        if (runtime->java_vm_ == vm) {
            return  &runtime->class_linker_->boot_class_path_;
        } else {
            size_t actual=0;
            for (int i = 0; i < 1024; i+=4) {
                if(*(JavaVM**)(((uint8_t*)(runtime))+i)==vm){
                    actual=i-(offsetof(Runtime,java_vm_)-offsetof(Runtime,class_linker_));
                    LOGE("Expected=%d,actual=%d",(int)actual,(int)offsetof(Runtime,class_linker_));
                    break;
                }
            }
            if(actual==0){
                LOGE("Not matched vm");
                return nullptr;
            }
            return  &(*reinterpret_cast<decltype(&runtime->class_linker_)>(reinterpret_cast<uint8_t *>(runtime)+actual))->boot_class_path_;
        }
    }

#if defined (__x86_64) || defined (__x86_64__)
#define ARCH "x86_64"
#elif defined(__i386__) || defined(__i386) || defined(__X86__)
#define ARCH "x86"
#elif defined(__aarch64__)
#define ARCH "arm64"
#elif defined(__arm__)
#define ARCH
#endif

    jobjectArray getAllBootClasses(JNIEnv *env, jclass) {
        if (sRuntime == nullptr) {
            LOGE("Runtime not initialized");
            return nullptr;
        }
        int sdk = getSDK();
        vector_base<const void *> *dexFiles;
        switch (sdk) {
            case 23:{
                dexFiles=getBootDexFiles<Runtime_23>();
                break;
            }
            case 24:
            case 25:{
                dexFiles=getBootDexFiles<Runtime_24_25>();
                break;
            }
            case 26: {
                dexFiles=getBootDexFiles<Runtime_26>();
                break;
            }
            case 27: {
                dexFiles=getBootDexFiles<Runtime_27>();
                break;
            }
            case 28:{
                dexFiles=getBootDexFiles<Runtime_28>();
                break;
            }
            default: {
                if (sdk < 29) {
                    LOGE("Unsupported version");
                    return nullptr;
                }
                dexFiles=getBootDexFiles<Runtime_29>();
                break;
            }
        }
        if(dexFiles== nullptr) return nullptr;
        return getClassList(env, dexFiles);
    }

    jobjectArray getClassList(JNIEnv *env, jclass, jobject cookie){
        int sdk=getSDK();
        if(sdk<21){
            uint8_t * dexBegin=(uint8_t*)env->GetDirectBufferAddress(cookie);
            if(strcmp((char*)dexBegin,"dey\n036")==0){//odex file
                struct DexOptHeader {
                    uint8_t magic[8];
                    uint32_t dexOffset;
                    uint32_t dexLength;
                }* pHeader= reinterpret_cast<DexOptHeader *>(dexBegin);
                dexBegin=dexBegin+pHeader->dexOffset;
            }else if(strcmp((char*)dexBegin,"dex\n035")!=0){
                return nullptr;//bad dex file
            }
            art::DexFile dexFile;
            dexFile.begin_=dexBegin;
            dexFile.header_=(art::Header*)dexBegin;
            dexFile.type_ids_= reinterpret_cast<art::TypeId *>(dexFile.begin_ + dexFile.header_->type_ids_off_);
            dexFile.string_ids_= reinterpret_cast<art::StringId *>(dexFile.begin_ + dexFile.header_->string_ids_off_);
            dexFile.class_defs_ = reinterpret_cast<art::ClassDef *>(dexFile.begin_ + dexFile.header_->class_defs_off_);
            void* arr[]={&dexFile};
            return getClassList(env,arr,1);
        };
        switch (sdk){
            case 21:
            case 22:
                return getClassList(env, reinterpret_cast<vector_base<const void*>*>(env->CallLongMethod(cookie,longValue)));
            default:
                int len=env->GetArrayLength((jlongArray)cookie);
                jlong *array=env->GetLongArrayElements((jlongArray)cookie,NULL);
                void* dexFiles[sdk==23?len:len-1];
                int total=0;
                for (int i = sdk==23?0:1; i < len; ++i) {
                    dexFiles[total++]=(void*)array[i];
                }
                env->ReleaseLongArrayElements((jlongArray)cookie,array,0);
                return getClassList(env, dexFiles,total);
        }

    }
}

JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved){
    JNIEnv *env;
    vm->GetEnv((void**)&env,JNI_VERSION_1_4);
    ::vm=vm;
    int sdk=getSDK();
    jclass resolverClass=env->FindClass("com/oslorde/dexresolver/DexResolver");
    jclass numberClass=env->FindClass("java/lang/Number");
    longValue=env->GetMethodID(numberClass,"longValue","()J");
    if(sdk>=23){
        DexResolver::init();
        JNINativeMethod method[]={JNINativeMethod{"getBootClassList","()[[Ljava/lang/String;",(void*)DexResolver::getAllBootClasses}};
        env->RegisterNatives(resolverClass,method,1);
    }
    JNINativeMethod method[]={JNINativeMethod{"getClassList","(Ljava/lang/Object;)[[Ljava/lang/String;",(void*)DexResolver::getClassList}};
    env->RegisterNatives(resolverClass,method,1);
    env->DeleteLocalRef(resolverClass);
    env->DeleteLocalRef(numberClass);
    return JNI_VERSION_1_4;
}

