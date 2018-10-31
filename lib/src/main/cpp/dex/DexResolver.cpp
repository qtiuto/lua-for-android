

#include <jni.h>
#include <vector>
#include <list>
#include <unordered_set>
#include <dlfcn.h>
#include <common.h>
#include <sys/mman.h>
#include "fake_dlfcn.h"
#include "dex_file.h"

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

struct ClassLinker_26 {
    std::vector<const void *> boot_class_path_;
};

struct ClassLinker_virtual {
    virtual ~ClassLinker_virtual() {}

    std::vector<const void *> boot_class_path_;
};
#define dexAccess(dex,member) (sdk<28?((art::DexFile*)dex)->member:((art::DexFile28*)dex)->member)

#define dexCall(dex,member, ...) (sdk<28?((art::DexFile*)dex)->member(__VA_ARGS__):((art::DexFile28*)dex)->member(__VA_ARGS__))

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

    std::string compiler_executable_;
    std::string patchoat_executable_;
    std::vector<std::string> compiler_options_;
    std::vector<std::string> image_compiler_options_;
    std::string image_location_;

    std::string boot_class_path_string_;
    std::string class_path_string_;
    std::vector<std::string> properties_;

    std::list<int> agents_;
    std::vector<int> plugins_;

    // The default stack size for managed threads created by the runtime.
    size_t default_stack_size_;

    void *heap_;

    std::unique_ptr<int> jit_arena_pool_;
    std::unique_ptr<int> arena_pool_;
    // Special low 4gb pool for compiler linear alloc. We need ArtFields to be in low 4gb if we are
    // compiling using a 32 bit image on a 64 bit compiler in case we resolve things in the image
    // since the field arrays are int arrays in this case.
    std::unique_ptr<int> low_4gb_arena_pool_;

    // Shared linear alloc for now.
    std::unique_ptr<int> linear_alloc_;

    // The number of spins that are done before thread suspension is used to forcibly inflate.
    size_t max_spins_before_thin_lock_inflation_;
    void *monitor_list_;
    void *monitor_pool_;

    void *thread_list_;

    void *intern_table_;

    ClassLinker_26 *class_linker_;

    void *signal_catcher_;
    std::string stack_trace_file_;

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

    std::string compiler_executable_;
    std::string patchoat_executable_;
    std::vector<std::string> compiler_options_;
    std::vector<std::string> image_compiler_options_;
    std::string image_location_;

    std::string boot_class_path_string_;
    std::string class_path_string_;
    std::vector<std::string> properties_;

    std::list<int> agents_;
    std::vector<int> plugins_;

    // The default stack size for managed threads created by the runtime.
    size_t default_stack_size_;

    void *heap_;

    std::unique_ptr<int> jit_arena_pool_;
    std::unique_ptr<int> arena_pool_;
    // Special low 4gb pool for compiler linear alloc. We need ArtFields to be in low 4gb if we are
    // compiling using a 32 bit image on a 64 bit compiler in case we resolve things in the image
    // since the field arrays are int arrays in this case.
    std::unique_ptr<int> low_4gb_arena_pool_;

    // Shared linear alloc for now.
    std::unique_ptr<int> linear_alloc_;

    // The number of spins that are done before thread suspension is used to forcibly inflate.
    size_t max_spins_before_thin_lock_inflation_;
    void *monitor_list_;
    void *monitor_pool_;

    void *thread_list_;

    void *intern_table_;

    ClassLinker_virtual *class_linker_;

    void *signal_catcher_;
    std::string stack_trace_file_;
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

    std::string compiler_executable_;
    std::string patchoat_executable_;
    std::vector<std::string> compiler_options_;
    std::vector<std::string> image_compiler_options_;
    std::string image_location_;

    std::string boot_class_path_string_;
    std::string class_path_string_;
    std::vector<std::string> properties_;

    std::list<int> agent_specs_;
    std::list<int> agents_;
    std::vector<int> plugins_;

    // The default stack size for managed threads created by the runtime.
    size_t default_stack_size_;

    void *heap_;

    std::unique_ptr<int> jit_arena_pool_;
    std::unique_ptr<int> arena_pool_;
    // Special low 4gb pool for compiler linear alloc. We need ArtFields to be in low 4gb if we are
    // compiling using a 32 bit image on a 64 bit compiler in case we resolve things in the image
    // since the field arrays are int arrays in this case.
    std::unique_ptr<int> low_4gb_arena_pool_;

    // Shared linear alloc for now.
    std::unique_ptr<int> linear_alloc_;

    // The number of spins that are done before thread suspension is used to forcibly inflate.
    size_t max_spins_before_thin_lock_inflation_;
    void *monitor_list_;
    void *monitor_pool_;

    void *thread_list_;

    void *intern_table_;

    ClassLinker_virtual *class_linker_;

    void *signal_catcher_;
    std::string stack_trace_file_;
    bool use_tombstoned_traces_;
    std::unique_ptr<JavaVM> java_vm_;
};
static void *runtime;
static void fixName(char* tmp, const char* src,int len){
    memcpy(tmp,src,(size_t)len);
    tmp[len]=0;
    for(int i=len-1;i!=-1;--i){
        if(tmp[i]=='/')
            tmp[i]='.';
    }
}
static inline void bestCache(char** orig,int* cacheLen,int len){
    if(len>*cacheLen){
        delete [] *orig;
        len=len+(len>>1);
        *orig=new char[len];
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
static jobjectArray getClassList(JNIEnv *env, const std::vector<const void *> *dexFiles) {
    jobjectArray ret = nullptr;
    char *tmp = new char[256];
    int sdk=getSDK();
    int cacheLen = 256;
    int index = 0;
    for (auto &&dexFile:*dexFiles) {
        int size = dexAccess(dexFile,header_)->class_defs_size_;
        JObjectArray classes(env,env->NewObjectArray(size,stringType,NULL));
        for (int i = 0; i <size; ++i ) {
            auto idx=dexAccess(dexFile,class_defs_)[i].class_idx_;
            const char *bytes = dexCall(dexFile,stringFromType,idx) + 1;
            int len = (int) strlen(bytes);
            bestCache(&tmp, &cacheLen, len);
            fixName(tmp, bytes, len - 1);
            env->SetObjectArrayElement(classes, i, JString(env, env->NewStringUTF(tmp)));
        }
        if(!ret) ret=env->NewObjectArray(dexFiles->size(), env->GetObjectClass(classes), NULL);
        env->SetObjectArrayElement(ret,index,classes);
        ++index;
    }
    delete tmp;
    return ret;
}
namespace DexResolver {
    void init() {
        int sdk = getSDK();
        void *handle = fake_dlopen("/system/" libPath "/libart.so", RTLD_NOW);
        runtime = *(void**)fake_dlsym(handle, "_ZN3art7Runtime9instance_E");
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



    jobjectArray getAllBootClasses(JNIEnv *env, jclass) {
        if (runtime == nullptr) {
            LOGE("Runtime not initialized");
            return nullptr;
        }
        int sdk = getSDK();
        std::vector<const void *> *dexFiles;
        switch (sdk) {
            case 26: {
                Runtime_26 *runtime_26 = static_cast<Runtime_26 *>(runtime);
                if (runtime_26->java_vm_.get() == vm) {
                    dexFiles = &runtime_26->class_linker_->boot_class_path_;
                } else {
                    size_t actual=0;
                    for (int i = 0; i < 1024; i+=4) {
                        if(*(JavaVM**)(((uint8_t*)(runtime_26))+i)==vm){
                            //LOGI("Expected =%d, Actual=%d",offsetof(Runtime_26,java_vm_),i);
                            actual=i-(offsetof(Runtime_26,java_vm_)-offsetof(Runtime_26,class_linker_));
                            break;
                        }
                    }
                    if(actual==0){
                        LOGE("Not matched vm");
                        return nullptr;
                    }
                    dexFiles= &(*reinterpret_cast<ClassLinker_virtual**>(reinterpret_cast<uint8_t *>(runtime_26)+actual))->boot_class_path_;
                }

                break;
            }
            case 27: {
                Runtime_27 *runtime_27 = static_cast<Runtime_27 *>(runtime);
                if (runtime_27->java_vm_.get() == vm) {
                    dexFiles = &runtime_27->class_linker_->boot_class_path_;
                } else {
                    size_t actual=0;
                    for (int i = 0; i < 1024; i+=4) {
                        if(*(JavaVM**)(((uint8_t*)(runtime_27))+i)==vm){
                            //LOGI("Expected =%d, Actual=%ld",offsetof(Runtime_27,java_vm_),i);
                            actual=i-(offsetof(Runtime_27,java_vm_)-offsetof(Runtime_27,class_linker_));
                            break;
                        }
                    }
                    if(actual==0){
                        LOGE("Not matched vm");
                        return nullptr;
                    }
                    dexFiles= &(*reinterpret_cast<ClassLinker_26**>(reinterpret_cast<uint8_t *>(runtime_27)+actual))->boot_class_path_;
                }
                break;
            }
            default: {
                if (sdk < 28) {
                    LOGE("Unsupported version");
                    return nullptr;
                }
                Runtime_28 *runtime_28 = static_cast<Runtime_28 *>(runtime);
                if (runtime_28->java_vm_.get() == vm) {
                    dexFiles = &runtime_28->class_linker_->boot_class_path_;
                } else {
                    size_t actual=0;
                    for (int i = 0; i < 1024; i+=4) {
                        if(*(JavaVM**)(((uint8_t*)(runtime_28))+i)==vm){
                            //LOGI("Expected =%d, Actual=%ld",offsetof(Runtime_28,java_vm_),i);
                            actual=i-(offsetof(Runtime_28,java_vm_)-offsetof(Runtime_28,class_linker_));
                            break;
                        }
                    }
                    if(actual==0){
                        LOGE("Not matched vm");
                        return nullptr;
                    }
                    dexFiles= &(*reinterpret_cast<ClassLinker_virtual**>(reinterpret_cast<uint8_t *>(runtime_28)+actual))->boot_class_path_;
                }
                break;
            }
        }
        return getClassList(env, dexFiles);
    }

    jobjectArray getClassList(JNIEnv *env, jclass, jobject cookie){
        int sdk=getSDK();
        if(sdk<21) return nullptr;
        std::vector<const void*> dexFiles;
        switch (sdk){
            case 21:
            case 22:
                 dexFiles= *reinterpret_cast<std::vector<const void*>*>(env->CallLongMethod(cookie,longValue));
                break;
            default:
                int len=env->GetArrayLength((jlongArray)cookie);
                jlong *array=env->GetLongArrayElements((jlongArray)cookie,NULL);
                dexFiles.reserve(sdk==23?len:len-1);
                for (int i = sdk==23?0:1; i < len; ++i) {
                    dexFiles.push_back((void*)array[i]);
                }
                env->ReleaseLongArrayElements((jlongArray)cookie,array,0);
                break;
        }
        return getClassList(env, &dexFiles);
    }
}
