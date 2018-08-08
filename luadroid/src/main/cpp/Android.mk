LOCAL_PATH := $(call my-dir)

ABI:=$(TARGET_ARCH_ABI)
ifeq ($(TARGET_ARCH_ABI),arm64-v8a)
    ABI:=arm64
endif
ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)
    ABI:=armv7
endif
include $(CLEAR_VARS)
LOCAL_MODULE := luajit
LOCAL_SRC_FILES := $(LOCAL_PATH)/luajit/$(ABI)/libluajit-5.1.a
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/luajit/include/luajit-2.1
include $(PREBUILT_STATIC_LIBRARY)
include $(CLEAR_VARS)
# 赋值给NDK编译系统
LOCAL_SRC_FILES  := java_type.cpp luadroid.cpp log_wrapper.cpp script_context.cpp utf8.cpp
LOCAL_C_INCLUDES := $(LOCAL_PATH)


LOCAL_STATIC_LIBRARIES :=lua
LOCAL_LDFLAGS += -Wl
LOCAL_LDLIBS := -ldl -llog -latomic
LOCAL_MODULE :=luadroid
$(info local c includes=$(LOCAL_C_INCLUDES))
$(info local path=$(LOCAL_PATH))
include $(BUILD_SHARED_LIBRARY)

$(call import-add-path,$(LOCAL_PATH))
$(call import-module, lua)