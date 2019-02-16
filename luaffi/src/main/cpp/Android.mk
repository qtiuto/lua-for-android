LOCAL_PATH := $(call my-dir)
LUA :=lua
ifeq (,$(wildcard $(LOCAL_PATH)/call_arm.h))
     $(shell $(LUA) $(LOCAL_PATH)/dynasm/dynasm.lua -LN -o $(LOCAL_PATH)/call_arm.h $(LOCAL_PATH)/call_arm.dasc)
endif
ifeq (,$(wildcard $(LOCAL_PATH)/call_arm_hf.h))
     $(shell $(LUA) $(LOCAL_PATH)/dynasm/dynasm.lua -LN -D ARM_HF -o $(LOCAL_PATH)/call_arm_hf.h $(LOCAL_PATH)/call_arm.dasc)
endif
ifeq (,$(wildcard $(LOCAL_PATH)/call_arm64.h))
     $(shell $(LUA) $(LOCAL_PATH)/dynasm/dynasm.lua -LN -o $(LOCAL_PATH)/call_arm64.h $(LOCAL_PATH)/call_arm64.dasc)
endif

ifeq (,$(wildcard $(LOCAL_PATH)/call_x86.h))
     $(shell $(LUA) $(LOCAL_PATH)/dynasm/dynasm.lua -LN  -o $(LOCAL_PATH)/call_x86.h $(LOCAL_PATH)/call_x86.dasc)
endif

ifeq (,$(wildcard $(LOCAL_PATH)/call_x64.h))
     $(shell $(LUA) $(LOCAL_PATH)/dynasm/dynasm.lua -LN -D X64 -o $(LOCAL_PATH)/call_x64.h $(LOCAL_PATH)/call_x86.dasc)
endif

ifeq (,$(wildcard $(LOCAL_PATH)/call_x64win.h))
     $(shell $(LUA) $(LOCAL_PATH)/dynasm/dynasm.lua -LN -D X64 -D X64WIN -o $(LOCAL_PATH)/call_x64win.h $(LOCAL_PATH)/call_x86.dasc)
endif

include $(CLEAR_VARS)
LOCAL_SRC_FILES  := ffi.c call.c parser.c ctype.c
LOCAL_C_INCLUDES := $(LOCAL_PATH) $(LOCAL_PATH)/../../../../lib/src/main/externalLib/lua
LOCAL_STATIC_LIBRARIES :=$(LUA_LIB)
LOCAL_LDLIBS := -ldl -llog
LOCAL_LDFLAGS := $(LOCAL_PATH)/../lua/$(TARGET_ARCH_ABI)/libluadroid.so
LOCAL_CFLAGS +=  -D FAKE_ANDROID_DL
LOCAL_MODULE :=ffi
$(info local c includes=$(LOCAL_C_INCLUDES))
$(info local path=$(LOCAL_PATH))
include $(BUILD_SHARED_LIBRARY)