LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES  := fake_dlfcn.c DexResolver.cpp
LOCAL_C_INCLUDES := $(LOCAL_PATH)
ifneq ($(APP_OPTIM),debug)
LOCAL_CFLAGS  += -Os -fno-math-errno -fvisibility=hidden
LOCAL_CPPFLAGS += -Os
LOCAL_LDFLAGS += -Wl,--gc-sections
endif
LOCAL_LDLIBS := -ldl -llog
LOCAL_MODULE :=dexresolver
$(info local c includes=$(LOCAL_C_INCLUDES))
$(info local path=$(LOCAL_PATH))
include $(BUILD_SHARED_LIBRARY)