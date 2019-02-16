LOCAL_PATH := $(call my-dir)
#never link to avoid git problems
include $(CLEAR_VARS)
LOCAL_SRC_FILES  := buffer.c auxiliar.c options.c timeout.c io.c \
                    usocket.c serial.c unixstream.c  unixdgram.c compat.c \
	                unix.c luasocket.c inet.c except.c select.c tcp.c udp.c
LOCAL_C_INCLUDES := $(LOCAL_PATH) $(LOCAL_PATH)/../../../../lib/src/main/externalLib/lua
LOCAL_STATIC_LIBRARIES :=$(LUA_LIB)
LOCAL_CFLAGS := -Os
LOCAL_LDFLAGS := $(LOCAL_PATH)/../../../../luaffi/src/main/lua/$(TARGET_ARCH_ABI)/libluadroid.so
LOCAL_MODULE :=socket
$(info local c includes=$(LOCAL_C_INCLUDES))
$(info local path=$(LOCAL_PATH))
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_SRC_FILES  := mime.c compat.c
LOCAL_C_INCLUDES := $(LOCAL_PATH) $(LOCAL_PATH)/../../../../lib/src/main/externalLib/lua
LOCAL_STATIC_LIBRARIES :=$(LUA_LIB)
LOCAL_CFLAGS := -Os
LOCAL_LDFLAGS := $(LOCAL_PATH)/../../../../luaffi/src/main/lua/$(TARGET_ARCH_ABI)/libluadroid.so
LOCAL_MODULE :=mime
$(info local c includes=$(LOCAL_C_INCLUDES))
$(info local path=$(LOCAL_PATH))
include $(BUILD_SHARED_LIBRARY)