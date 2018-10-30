/*
 * Copyright (C) 2011 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ART_RUNTIME_DEX_FILE_H_
#define ART_RUNTIME_DEX_FILE_H_

#include <memory>
#include <string>
#include <atomic>

#include "jni.h"
#include "modifiers.h"
#include "macros.h"


typedef uint8_t byte;
#define PACKED(x) __attribute__ ((__aligned__(x), __packed__))
namespace art {
    enum InvokeType {
        kStatic,     // <<static>>
        kDirect,     // <<direct>>
        kVirtual,    // <<virtual>>
        kSuper,      // <<super>>
        kInterface,  // <<interface>>
        kMaxInvokeType = kInterface
    };
    static constexpr size_t kSha1DigestSize = 20;

    struct Header {
        uint8_t magic_[8];
        uint32_t checksum_;  // See also location_checksum_
        uint8_t signature_[kSha1DigestSize];
        uint32_t file_size_;  // size of entire file
        uint32_t header_size_;  // offset to start of next section
        uint32_t endian_tag_;
        uint32_t link_size_;  // unused
        uint32_t link_off_;  // unused
        uint32_t map_off_;  // unused
        uint32_t string_ids_size_;  // number of StringIds
        uint32_t string_ids_off_;  // file offset of StringIds array
        uint32_t type_ids_size_;  // number of TypeIds, we don't support more than 65535
        uint32_t type_ids_off_;  // file offset of TypeIds array
        uint32_t proto_ids_size_;  // number of ProtoIds, we don't support more than 65535
        uint32_t proto_ids_off_;  // file offset of ProtoIds array
        uint32_t field_ids_size_;  // number of FieldIds
        uint32_t field_ids_off_;  // file offset of FieldIds array
        uint32_t method_ids_size_;  // number of MethodIds
        uint32_t method_ids_off_;  // file offset of MethodIds array
        uint32_t class_defs_size_;  // number of ClassDefs
        uint32_t class_defs_off_;  // file offset of ClassDef array
        uint32_t data_size_;  // unused
        uint32_t data_off_;  // unused
    };

    // Map item type codes.
    enum {
        kDexTypeHeaderItem = 0x0000,
        kDexTypeStringIdItem = 0x0001,
        kDexTypeTypeIdItem = 0x0002,
        kDexTypeProtoIdItem = 0x0003,
        kDexTypeFieldIdItem = 0x0004,
        kDexTypeMethodIdItem = 0x0005,
        kDexTypeClassDefItem = 0x0006,
        kDexTypeCallSiteIdItem           = 0x0007,
        kDexTypeMethodHandleItem         = 0x0008,
        kDexTypeMapList = 0x1000,
        kDexTypeTypeList = 0x1001,
        kDexTypeAnnotationSetRefList = 0x1002,
        kDexTypeAnnotationSetItem = 0x1003,
        kDexTypeClassDataItem = 0x2000,
        kDexTypeCodeItem = 0x2001,
        kDexTypeStringDataItem = 0x2002,
        kDexTypeDebugInfoItem = 0x2003,
        kDexTypeAnnotationItem = 0x2004,
        kDexTypeEncodedArrayItem = 0x2005,
        kDexTypeAnnotationsDirectoryItem = 0x2006,
    };

    struct MapItem {
        uint16_t type_;
        uint16_t unused_;
        uint32_t size_;
        uint32_t offset_;

    };

    struct MapList {
        uint32_t size_;
        MapItem list_[1];

    };

    // Raw string_id_item.
    struct StringId {
        uint32_t string_data_off_;  // offset in bytes from the base address
    };

    // Raw type_id_item.
    struct TypeId {
        uint32_t descriptor_idx_;  // index into string_ids
    };

    // Raw field_id_item.
    struct FieldId {
        uint16_t class_idx_;  // index into type_ids_ array for defining class
        uint16_t type_idx_;  // index into type_ids_ array for field type
        uint32_t name_idx_;  // index into string_ids_ array for field name
    };

    // Raw method_id_item.
    struct MethodId {
        uint16_t class_idx_;  // index into type_ids_ array for defining class
        uint16_t proto_idx_;  // index into proto_ids_ array for method prototype
        uint32_t name_idx_;  // index into string_ids_ array for method name
    };

    // Raw proto_id_item.
    struct ProtoId {
        uint32_t shorty_idx_;  // index into string_ids array for shorty descriptor
        uint16_t return_type_idx_;  // index into type_ids array for return type
        uint16_t pad_;             // padding = 0
        uint32_t parameters_off_;  // file offset to type_list for parameter types
    };

    // Raw class_def_item.
    struct ClassDef {
        uint16_t class_idx_;  // index into type_ids_ array for this class
        uint16_t pad1_;  // padding = 0
        uint32_t access_flags_;
        uint16_t superclass_idx_;  // index into type_ids_ array for superclass
        uint16_t pad2_;  // padding = 0
        uint32_t interfaces_off_;  // file offset to TypeList
        uint32_t source_file_idx_;  // index into string_ids_ for source file name
        uint32_t annotations_off_;  // file offset to annotations_directory_item
        uint32_t class_data_off_;  // file offset to class_data_item
        uint32_t static_values_off_;  // file offset to EncodedArray

        // Returns the valid access flags, that is, Java modifier bits relevant to the ClassDef type
        // (class or interface). These are all in the lower 16b and do not contain runtime flags.
        uint32_t GetJavaAccessFlags() {
            if ((access_flags_ & kAccInterface) != 0) {
                // Interface.
                return access_flags_ & kAccValidInterfaceFlags;
            } else {
                // Class.
                return access_flags_ & kAccValidClassFlags;
            }
        }


    };

    // Raw type_item.
    struct TypeItem {
        uint16_t type_idx_;  // index into type_ids section
    };

    // Raw type_list.
    class TypeList {
    private:
        uint32_t size_;  // size of the list, in entries
        TypeItem list_[1];  // elements of the list

    public:
        uint32_t Size() {
            return size_;
        }

        TypeItem &GetTypeItem(uint32_t idx) {
            return this->list_[idx];
        }

        // Size in bytes of the part of the list that is common.
        static constexpr size_t GetHeaderSize() {
            return 4U;
        }

        // Size in bytes of the whole type list including all the stored elements.
        static constexpr size_t GetListSize(size_t count) {
            return GetHeaderSize() + sizeof(TypeItem) * count;
        }


    };

    // Raw code_item.
    struct CodeItem {
        uint16_t registers_size_;
        uint16_t ins_size_;
        uint16_t outs_size_;
        uint16_t tries_size_;
        uint32_t debug_info_off_;  // file offset to debug info stream
        uint32_t insns_size_in_code_units_;  // size of the insns array, in 2 byte code units
        uint16_t insns_[1];

        void toString() {
            LOGV("registers_size_=%d,"
                 "ins_size_=%d,"
                 "outs_size_=%d,"
                 "tries_size_=%d,"
                 "debug_info_off_=%d,"
                 "insns_size_in_code_units_=%d",
                 registers_size_,
                 ins_size_,
                 outs_size_,
                 tries_size_,
                 debug_info_off_,
                 insns_size_in_code_units_);
        }
    } PACKED(1);//to avoid default 4-byte aligned imposed by the complier,which cause sizeof return 20 rather than true size 18
    // Raw try_item.
    struct TryItem {
        uint32_t start_addr_;
        uint16_t insn_count_;
        uint16_t handler_off_;
    };


    // Annotation constants.
    enum {
        kDexVisibilityBuild = 0x00, /* annotation visibility */
        kDexVisibilityRuntime = 0x01,
        kDexVisibilitySystem = 0x02,

        kDexAnnotationByte = 0x00,
        kDexAnnotationShort = 0x02,
        kDexAnnotationChar = 0x03,
        kDexAnnotationInt = 0x04,
        kDexAnnotationLong = 0x06,
        kDexAnnotationFloat = 0x10,
        kDexAnnotationDouble = 0x11,
        kDexAnnotationString = 0x17,
        kDexAnnotationType = 0x18,
        kDexAnnotationField = 0x19,
        kDexAnnotationMethod = 0x1a,
        kDexAnnotationEnum = 0x1b,
        kDexAnnotationArray = 0x1c,
        kDexAnnotationAnnotation = 0x1d,
        kDexAnnotationNull = 0x1e,
        kDexAnnotationBoolean = 0x1f,

        kDexAnnotationValueTypeMask = 0x1f, /* low 5 bits */
        kDexAnnotationValueArgShift = 5,
    };

    struct AnnotationsDirectoryItem {
        uint32_t class_annotations_off_;
        uint32_t fields_size_;
        uint32_t methods_size_;
        uint32_t parameters_size_;
    };

    struct FieldAnnotationsItem {
        uint32_t field_idx_;
        uint32_t annotations_off_;
    };

    struct MethodAnnotationsItem {
        uint32_t method_idx_;
        uint32_t annotations_off_;
    };

    struct ParameterAnnotationsItem {
        uint32_t method_idx_;
        uint32_t annotations_off_;
    };

    struct AnnotationSetRefItem {
        uint32_t annotations_off_;
    };

    struct AnnotationSetRefList {
        uint32_t size_;
        AnnotationSetRefItem list_[1];
    };

    struct AnnotationSetItem {
        uint32_t size_;
        uint32_t entries_[1];
    };

    struct AnnotationItem {
        uint8_t visibility_;
        uint8_t annotation_[1];
    };
    static inline void skipULeb128(uint8_t *&ptr) {
        if (*ptr++ > 0x7f) {
            if (*ptr++ > 0x7f) {
                if (*ptr++ > 0x7f) {
                    if (*ptr++ > 0x7f) {
                        /*
                         * Note: We don't check to see if cur is out of
                         * range here, meaning we tolerate garbage in the
                         * high four-order bits.
                         */
                        ++ptr;
                    }
                }
            }
        }
    }
    class DexFile {
    public:


        // Do nothing
        virtual ~DexFile();


        // The base address of the memory mapping.
        uint8_t *begin_;

        // The size of the underlying memory allocation in bytes.
        size_t size_;
        // The base address of the data section (same as Begin() for standard dex).

        // Typically the dex file name when available, alternatively some identifying string.
        // The ClassLinker will use this to match DexFiles the boot class
        // path to DexCache::GetLocation when loading from an image.

        std::string location_;


        uint32_t location_checksum_;

        // Manages the underlying memory allocation.
        std::unique_ptr<void > mem_map_;

        // Points to the header section.
        Header *header_;

        // Points to the base of the string identifier list.
        StringId *string_ids_;

        // Points to the base of the type identifier list.
        TypeId *type_ids_;

        // Points to the base of the field identifier list.
        FieldId *field_ids_;

        // Points to the base of the method identifier list.
        MethodId *method_ids_;

        // Points to the base of the prototype identifier list.
        ProtoId *proto_ids_;

        // Points to the base of the class definition list.
        ClassDef *class_defs_;

        // Number of misses finding a class def from a descriptor.
        mutable std::atomic<uint32_t> find_class_def_misses_;


        char *stringByIndex(uint32_t index) {
            if (index >= header_->string_ids_size_) {
                LOGE("std::out_of_range:MethodIdx:%d", index);
                abort();
            }
            byte *ptr = begin_ + string_ids_[index].string_data_off_;
            skipULeb128(ptr);
            return (char *) ptr;
        }

        char *stringFromType(uint32_t index) {
            if (index >= header_->type_ids_size_) {
                LOGE("std::out_of_range:TypeIndex:%d", index);
                abort();
            }
            return stringByIndex(type_ids_[index].descriptor_idx_);
        }

    private:


    };

    class DexFile28 {
    public:
        // Do nothing
        virtual ~DexFile28() ;


        // The base address of the memory mapping.
        uint8_t *begin_;

        // The size of the underlying memory allocation in bytes.
        size_t size_;
        // The base address of the data section (same as Begin() for standard dex).
        uint8_t*  data_begin_;

        // The size of the data section.
        size_t data_size_;
        // Typically the dex file name when available, alternatively some identifying string.
        // The ClassLinker will use this to match DexFiles the boot class
        // path to DexCache::GetLocation when loading from an image.

        std::string location_;


        uint32_t location_checksum_;

        // Manages the underlying memory allocation.
        //std::unique_ptr<MemMap> mem_map_;

        // Points to the header section.
        Header *header_;

        // Points to the base of the string identifier list.
        StringId *string_ids_;

        // Points to the base of the type identifier list.
        TypeId *type_ids_;

        // Points to the base of the field identifier list.
        FieldId *field_ids_;

        // Points to the base of the method identifier list.
        MethodId *method_ids_;

        // Points to the base of the prototype identifier list.
        ProtoId *proto_ids_;

        // Points to the base of the class definition list.
        ClassDef *class_defs_;

        // Number of misses finding a class def from a descriptor.
        mutable std::atomic<uint32_t> find_class_def_misses_;

        char *stringByIndex(uint32_t index) {
            if (index >= header_->string_ids_size_) {
                LOGE("std::out_of_range:MethodIdx:%d", index);
                abort();
            }
            byte *ptr = data_begin_ + string_ids_[index].string_data_off_;
            skipULeb128(ptr);
            return (char *) ptr;
        }

        char *stringFromType(uint32_t index) {
            if (index >= header_->type_ids_size_) {
                LOGE("std::out_of_range:TypeIndex:%d", index);
                abort();
            }
            return stringByIndex(type_ids_[index].descriptor_idx_);
        }

    private:
    }; // Raw header_item.
}

//
#endif  // ART_RUNTIME_DEX_FILE_H_

