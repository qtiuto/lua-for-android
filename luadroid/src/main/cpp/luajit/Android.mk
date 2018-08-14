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
LOCAL_SRC_FILES := $(LOCAL_PATH)/$(ABI)/libluajit-5.1.a
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include/luajit-2.1
include $(PREBUILT_STATIC_LIBRARY)