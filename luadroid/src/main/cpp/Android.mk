LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES  := java_type.cpp luadroid.cpp log_wrapper.cpp script_context.cpp utf8.cpp
LOCAL_C_INCLUDES := $(LOCAL_PATH)

#LOCAL_STATIC_LIBRARIES :=luajit
#use the below statement if you intend to use standard lua
LOCAL_STATIC_LIBRARIES :=lua
LOCAL_LDFLAGS += -Wl
LOCAL_LDLIBS := -ldl -llog -latomic
LOCAL_MODULE :=luadroid
$(info local c includes=$(LOCAL_C_INCLUDES))
$(info local path=$(LOCAL_PATH))
include $(BUILD_SHARED_LIBRARY)

$(call import-add-path,$(LOCAL_PATH))
$(call import-module, lua)
$(call import-module, luajit)