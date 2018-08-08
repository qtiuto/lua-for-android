LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
# 配置自己的源文件目录和源文件后缀名
MY_FILES_PATH  :=  $(LOCAL_PATH)

MY_FILES_SUFFIX := %.c

# 递归遍历目录下的所有的文件
rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))

# 获取相应的源文件
MY_ALL_FILES := $(foreach src_path,$(MY_FILES_PATH), $(call rwildcard,$(src_path),*.*) )
MY_ALL_FILES := $(MY_ALL_FILES:$(MY_CPP_PATH)/./%=$(MY_CPP_PATH)%)
MY_SRC_LIST  := $(filter $(MY_FILES_SUFFIX),$(MY_ALL_FILES))
MY_SRC_LIST  := $(MY_SRC_LIST:$(LOCAL_PATH)/%=%)

# 去除字串的重复单词
define uniq =
  $(eval seen :=)
  $(foreach _,$1,$(if $(filter $_,${seen}),,$(eval seen += $_)))
  ${seen}
endef

# 递归遍历获取所有目录
MY_ALL_DIRS := $(dir $(foreach src_path,$(MY_FILES_PATH), $(call rwildcard,$(src_path),*/) ) )
MY_ALL_DIRS := $(call uniq,$(MY_ALL_DIRS))

EXPLICIT_SRC_EXCLUDES := %lua.c %luac.c
LOCAL_SRC_FILES := $(filter-out $(EXPLICIT_SRC_EXCLUDES), $(MY_SRC_LIST))
LOCAL_C_INCLUDES := $(MY_ALL_DIRS)
LOCAL_CFLAGS += -DLUA_USE_LINUX -O2 -Wall -DLUA_USE_LONGJMP -DLUA_COMPAT_5_2 -DLUA_COMPAT_5_1
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)
LOCAL_MODULE :=lua
$(info local c includes=$(LOCAL_C_INCLUDES))
$(info local path=$(LOCAL_PATH))
include $(BUILD_STATIC_LIBRARY)