

#ifndef LUADROID_RUNTIME_H
#define LUADROID_RUNTIME_H

#include "vector_base.h"
#include "macros.h"
#include "jtype.h"
#include "dex_file.h"

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

    JavaVM* java_vm_;
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

    JavaVM* java_vm_;
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

    JavaVM* java_vm_;
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
    JavaVM* java_vm_;
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
    JavaVM* java_vm_;
};
struct Runtime_29 {
    uint64_t callee_save_methods_[6];
    uint32_t pre_allocated_OutOfMemoryError_when_throwing_exception_;
    uint32_t pre_allocated_OutOfMemoryError_when_throwing_oome_;
    uint32_t pre_allocated_OutOfMemoryError_when_handling_stack_overflow_;
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
    //QuickMethodFrameInfo callee_save_method_frame_infos_[6];

    void *compiler_callbacks_;
    bool is_zygote_;
    bool must_relocate_;
    bool is_concurrent_gc_enabled_;
    bool is_explicit_gc_disabled_;
    bool image_dex2oat_enabled_;

    StringStub compiler_executable_;
    vector_base<StringStub> compiler_options_;
    vector_base<StringStub> image_compiler_options_;
    StringStub image_location_;
    bool is_using_default_boot_image_location_;

    vector_base<StringStub> boot_class_path_;
    vector_base<StringStub> boot_class_path_locations_;
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
    JavaVM* java_vm_;
};
#endif //LUADROID_RUNTIME_H
