LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
# 配置自己的源文件目录和源文件后缀名
MY_FILES_PATH  :=  $(LOCAL_PATH)

MY_FILES_SUFFIX := %.c
EXPLICIT_SRC_EXCLUDES := %lua.c %luac.c

# 获取相应的源文件
MY_ALL_FILES := $(wildcard $(LOCAL_PATH)/*.*))
MY_SRC_LIST  := $(filter $(MY_FILES_SUFFIX),$(MY_ALL_FILES))
MY_SRC_LIST  := $(filter-out $(EXPLICIT_SRC_EXCLUDES), $(MY_SRC_LIST))

LOCAL_SRC_FILES := $(MY_SRC_LIST:$(LOCAL_PATH)/%=%)
LOCAL_C_INCLUDES := $(LOCAL_PATH)
LOCAL_CFLAGS += -DLUA_USE_LINUX -O2 -Wall -DLUA_USE_LONGJMP -DLUA_COMPAT_5_2 -DLUA_COMPAT_5_1
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)
LOCAL_MODULE :=lua
$(info local c includes=$(LOCAL_C_INCLUDES))
$(info local path=$(LOCAL_PATH))
include $(BUILD_STATIC_LIBRARY)