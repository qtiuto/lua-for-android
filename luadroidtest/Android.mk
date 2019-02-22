LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_SRC_FILES  := ../luaffi/src/main/cpp/test.c
LOCAL_C_INCLUDES := $(LOCAL_PATH) $(LOCAL_PATH)/../lib/src/main/externalLib/lua
LOCAL_STATIC_LIBRARIES :=$(LUA_LIB)
LOCAL_LDFLAGS := $(LOCAL_PATH)/../luaffi/src/main/lua/$(TARGET_ARCH_ABI)/libluadroid.so
LOCAL_CFLAGS +=  -Wno-format
LOCAL_MODULE :=ffitest
include $(BUILD_SHARED_LIBRARY)