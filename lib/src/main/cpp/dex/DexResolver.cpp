

#include <jni.h>
#include <dlfcn.h>
#include <common.h>
#include <sys/mman.h>
#include "fake_dlfcn.h"
#include "dex_file.h"
#include <errno.h>
#include "vector_base.h"
#ifdef __LP64__
#define libPath "lib64"
#else
#define libPath "lib"
#endif

class PACKED(4) QuickMethodFrameInfo {
public:
    uint32_t frame_size_in_bytes_;
    uint32_t core_spill_mask_;
    uint32_t fp_spill_mask_;
};

struct ClassLinker {
    vector_base<const void *> boot_class_path_;
};

struct ClassLinker_virtual {
    virtual ~ClassLinker_virtual() {}

    vector_base<const void *> boot_class_path_;
};

template <typename T>
class list_stub{
    void* st[3];
};



#define dexAccess(dex,member) (sdk<28?((art::DexFile*)dex)->member:((art::DexFile28*)dex)->member)

#define dexCall(dex,member, ...) (sdk<28?((art::DexFile*)dex)->member(__VA_ARGS__):((art::DexFile28*)dex)->member(__VA_ARGS__))
struct Runtime_23{
    uint64_t callee_save_methods_[3];
    uint pre_allocated_OutOfMemoryError_;
    uint pre_allocated_NoClassDefFoundError_;
    void* resolution_method_;
    void* imt_conflict_method_;
    // Unresolved method has the same behavior as the conflict method, it is used by the class linker
    // for differentiating between unfilled imt slots vs conflict slots in superclasses.
    void* imt_unimplemented_method_;

    // Special sentinel object used to invalid conditions in JNI (cleared weak references) and
    // JDWP (invalid references).
    uint sentinel_;

    int instruction_set_;
    QuickMethodFrameInfo callee_save_method_frame_infos_[3];

    void * compiler_callbacks_;

    bool is_zygote_;
    bool must_relocate_;
    bool is_concurrent_gc_enabled_;
    bool is_explicit_gc_disabled_;
    bool dex2oat_enabled_;
    bool image_dex2oat_enabled_;

    StringStub compiler_executable_;
    StringStub patchoat_executable_;
    vector_base<StringStub> compiler_options_;
    vector_base<StringStub> image_compiler_options_;
    StringStub image_location_;

    StringStub boot_class_path_string_;
    StringStub class_path_string_;
    vector_base<StringStub> properties_;

    // The default stack size for managed threads created by the runtime.
    size_t default_stack_size_;

    void* heap_;

    void* arena_pool_;
    // Special low 4gb pool for compiler linear alloc. We need ArtFields to be in low 4gb if we are
    // compiling using a 32 bit image on a 64 bit compiler in case we resolve things in the image
    // since the field arrays are int arrays in this case.
    void* low_4gb_arena_pool_;

    // Shared linear alloc for now.
    void* linear_alloc_;

    // The number of spins that are done before thread suspension is used to forcibly inflate.
    size_t max_spins_before_thin_lock_inflation_;
    void* monitor_list_;
    void* monitor_pool_;

    void* thread_list_;

    void* intern_table_;

    ClassLinker* class_linker_;

    void* signal_catcher_;
    StringStub stack_trace_file_;

    std::unique_ptr<JavaVM> java_vm_;
};
struct Runtime_24_25{
    uint64_t callee_save_methods_[3];
    uint pre_allocated_OutOfMemoryError_;
    uint pre_allocated_NoClassDefFoundError_;
    void* resolution_method_;
    void* imt_conflict_method_;
    // Unresolved method has the same behavior as the conflict method, it is used by the class linker
    // for differentiating between unfilled imt slots vs conflict slots in superclasses.
    void* imt_unimplemented_method_;

    // Special sentinel object used to invalid conditions in JNI (cleared weak references) and
    // JDWP (invalid references).
    uint sentinel_;

    int instruction_set_;
    QuickMethodFrameInfo callee_save_method_frame_infos_[3];

    void * compiler_callbacks_;

    bool is_zygote_;
    bool must_relocate_;
    bool is_concurrent_gc_enabled_;
    bool is_explicit_gc_disabled_;
    bool dex2oat_enabled_;
    bool image_dex2oat_enabled_;

    StringStub compiler_executable_;
    StringStub patchoat_executable_;
    vector_base<StringStub> compiler_options_;
    vector_base<StringStub> image_compiler_options_;
    StringStub image_location_;

    StringStub boot_class_path_string_;
    StringStub class_path_string_;
    vector_base<StringStub> properties_;

    // The default stack size for managed threads created by the runtime.
    size_t default_stack_size_;

    void* heap_;

    void* jit_arena_pool_;
    void* arena_pool_;
    // Special low 4gb pool for compiler linear alloc. We need ArtFields to be in low 4gb if we are
    // compiling using a 32 bit image on a 64 bit compiler in case we resolve things in the image
    // since the field arrays are int arrays in this case.
    void* low_4gb_arena_pool_;

    // Shared linear alloc for now.
    void* linear_alloc_;

    // The number of spins that are done before thread suspension is used to forcibly inflate.
    size_t max_spins_before_thin_lock_inflation_;
    void* monitor_list_;
    void* monitor_pool_;

    void* thread_list_;

    void* intern_table_;

    ClassLinker* class_linker_;

    void* signal_catcher_;
    StringStub stack_trace_file_;

    std::unique_ptr<JavaVM> java_vm_;
};
struct Runtime_26 {
    uint64_t callee_save_methods_[4];
    uint32_t pre_allocated_OutOfMemoryError_;
    uint32_t pre_allocated_NoClassDefFoundError_;
    void *resolution_method_;
    void *imt_conflict_method_;
    // Unresolved method has the same behavior as the conflict method, it is used by the class linker
    // for differentiating between unfilled imt slots vs conflict slots in superclasses.
    void *imt_unimplemented_method_;

    // Special sentinel object used to invalid conditions in JNI (cleared weak references) and
    // JDWP (invalid references).
    uint32_t sentinel_;

    int instruction_set_;
    QuickMethodFrameInfo callee_save_method_frame_infos_[4];

    void *compiler_callbacks_;
    bool is_zygote_;
    bool must_relocate_;
    bool is_concurrent_gc_enabled_;
    bool is_explicit_gc_disabled_;
    bool dex2oat_enabled_;
    bool image_dex2oat_enabled_;

    StringStub compiler_executable_;
    StringStub patchoat_executable_;
    vector_base<StringStub> compiler_options_;
    vector_base<StringStub> image_compiler_options_;
    StringStub image_location_;

    StringStub boot_class_path_string_;
    StringStub class_path_string_;
    vector_base<StringStub> properties_;

    list_stub<int> agents_;
    vector_base<int> plugins_;

    // The default stack size for managed threads created by the runtime.
    size_t default_stack_size_;

    void *heap_;

    void* jit_arena_pool_;
    void* arena_pool_;
    // Special low 4gb pool for compiler linear alloc. We need ArtFields to be in low 4gb if we are
    // compiling using a 32 bit image on a 64 bit compiler in case we resolve things in the image
    // since the field arrays are int arrays in this case.
    void* low_4gb_arena_pool_;

    // Shared linear alloc for now.
    void* linear_alloc_;

    // The number of spins that are done before thread suspension is used to forcibly inflate.
    size_t max_spins_before_thin_lock_inflation_;
    void *monitor_list_;
    void *monitor_pool_;

    void *thread_list_;

    void *intern_table_;

    ClassLinker *class_linker_;

    void *signal_catcher_;
    StringStub stack_trace_file_;

    std::unique_ptr<JavaVM> java_vm_;
};
struct Runtime_27 {
    uint64_t callee_save_methods_[6];
    uint32_t pre_allocated_OutOfMemoryError_;
    uint32_t pre_allocated_NoClassDefFoundError_;
    void *resolution_method_;
    void *imt_conflict_method_;
    // Unresolved method has the same behavior as the conflict method, it is used by the class linker
    // for differentiating between unfilled imt slots vs conflict slots in superclasses.
    void *imt_unimplemented_method_;

    // Special sentinel object used to invalid conditions in JNI (cleared weak references) and
    // JDWP (invalid references).
    uint32_t sentinel_;

    int instruction_set_;
    QuickMethodFrameInfo callee_save_method_frame_infos_[6];

    void *compiler_callbacks_;
    bool is_zygote_;
    bool must_relocate_;
    bool is_concurrent_gc_enabled_;
    bool is_explicit_gc_disabled_;
    bool dex2oat_enabled_;
    bool image_dex2oat_enabled_;

    StringStub compiler_executable_;
    StringStub patchoat_executable_;
    vector_base<StringStub> compiler_options_;
    vector_base<StringStub> image_compiler_options_;
    StringStub image_location_;

    StringStub boot_class_path_string_;
    StringStub class_path_string_;
    vector_base<StringStub> properties_;

    list_stub<int> agents_;
    vector_base<int> plugins_;

    // The default stack size for managed threads created by the runtime.
    size_t default_stack_size_;

    void *heap_;

    void* jit_arena_pool_;
    void* arena_pool_;
    // Special low 4gb pool for compiler linear alloc. We need ArtFields to be in low 4gb if we are
    // compiling using a 32 bit image on a 64 bit compiler in case we resolve things in the image
    // since the field arrays are int arrays in this case.
    void* low_4gb_arena_pool_;

    // Shared linear alloc for now.
    void* linear_alloc_;

    // The number of spins that are done before thread suspension is used to forcibly inflate.
    size_t max_spins_before_thin_lock_inflation_;
    void *monitor_list_;
    void *monitor_pool_;

    void *thread_list_;

    void *intern_table_;

    ClassLinker_virtual *class_linker_;

    void *signal_catcher_;
    StringStub stack_trace_file_;
    bool use_tombstoned_traces_;
    std::unique_ptr<JavaVM> java_vm_;
};
struct Runtime_28 {
    uint64_t callee_save_methods_[6];
    uint32_t pre_allocated_OutOfMemoryError_;
    uint32_t pre_allocated_NoClassDefFoundError_;
    void *resolution_method_;
    void *imt_conflict_method_;
    // Unresolved method has the same behavior as the conflict method, it is used by the class linker
    // for differentiating between unfilled imt slots vs conflict slots in superclasses.
    void *imt_unimplemented_method_;

    // Special sentinel object used to invalid conditions in JNI (cleared weak references) and
    // JDWP (invalid references).
    uint32_t sentinel_;

    int instruction_set_;
    QuickMethodFrameInfo callee_save_method_frame_infos_[6];

    void *compiler_callbacks_;
    bool is_zygote_;
    bool must_relocate_;
    bool is_concurrent_gc_enabled_;
    bool is_explicit_gc_disabled_;
    bool dex2oat_enabled_;
    bool image_dex2oat_enabled_;

    StringStub compiler_executable_;
    StringStub patchoat_executable_;
    vector_base<StringStub> compiler_options_;
    vector_base<StringStub> image_compiler_options_;
    StringStub image_location_;

    StringStub boot_class_path_string_;
    StringStub class_path_string_;
    vector_base<StringStub> properties_;

    list_stub<int> agent_specs_;
    list_stub<int> agents_;
    vector_base<int> plugins_;

    // The default stack size for managed threads created by the runtime.
    size_t default_stack_size_;

    void *heap_;

    void* jit_arena_pool_;
    void* arena_pool_;
    // Special low 4gb pool for compiler linear alloc. We need ArtFields to be in low 4gb if we are
    // compiling using a 32 bit image on a 64 bit compiler in case we resolve things in the image
    // since the field arrays are int arrays in this case.
    void* low_4gb_arena_pool_;

    // Shared linear alloc for now.
    void* linear_alloc_;

    // The number of spins that are done before thread suspension is used to forcibly inflate.
    size_t max_spins_before_thin_lock_inflation_;
    void *monitor_list_;
    void *monitor_pool_;

    void *thread_list_;

    void *intern_table_;

    ClassLinker_virtual *class_linker_;

    void *signal_catcher_;
    StringStub stack_trace_file_;
    bool use_tombstoned_traces_;
    std::unique_ptr<JavaVM> java_vm_;
};
static void *sRuntime;
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

static jobjectArray getClassList(JNIEnv *env,  const void *const *dexFiles,int length) {
    jobjectArray ret = nullptr;
    char *tmp = new char[256];
    int cacheLen = 256;
    int sdk=getSDK();
    int cacheIndexLen=dexAccess((dexFiles[0]),header_)->class_defs_size_;
    uint* idxes=new uint[cacheIndexLen];// for worst
    int index = 0;
    for (int k=0;k<length;++k) {
        auto dexFile=dexFiles[k];
        int size = dexAccess(dexFile,header_)->class_defs_size_;
        bestCache(&idxes, &cacheIndexLen, size);
        for (int i = 0; i <size; ++i ) {
            idxes[i]=dexAccess(dexFile,class_defs_)[i].class_idx_;
        }
        qsort(idxes,size, sizeof(uint),compareInt);
        JObjectArray classes(env,env->NewObjectArray(size,stringType,NULL));
        for (int i = 0; i <size; ++i ) {
            const char *bytes = dexCall(dexFile,stringFromType,idxes[i]) + 1;
            int len = (int) strlen(bytes);
            bestCache(&tmp, &cacheLen, len);
            fixName(tmp, bytes, len - 1);
            env->SetObjectArrayElement(classes, i, JString(env, env->NewStringUTF(tmp)));
        }
        if(!ret) ret=env->NewObjectArray(length, env->GetObjectClass(classes), NULL);
        env->SetObjectArrayElement(ret,index,classes);
        ++index;
    }
    delete[] idxes;
    delete[] tmp;
    return ret;
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
        if (runtime->java_vm_.get() == vm) {
            return  &runtime->class_linker_->boot_class_path_;
        } else {
            size_t actual=0;
            for (int i = 0; i < 1024; i+=4) {
                if(*(JavaVM**)(((uint8_t*)(runtime))+i)==vm){
                    actual=i-(offsetof(Runtime,java_vm_)-offsetof(Runtime,class_linker_));
                    LOGE("Expected=%d,actual=%d",actual,offsetof(Runtime,class_linker_));
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
            default: {
                if (sdk < 28) {
                    LOGE("Unsupported version");
                    return nullptr;
                }
                dexFiles=getBootDexFiles<Runtime_28>();
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