LOCAL_PATH := $(call my-dir)


include $(CLEAR_VARS)
LOCAL_SRC_FILES  := ssl.c ec.c x509.c config.c context.c \
                    luasocket/buffer.c luasocket/io.c luasocket/usocket.c luasocket/timeout.c
LOCAL_C_INCLUDES := $(LOCAL_PATH) $(LOCAL_PATH)/../../../../lib/src/main/externalLib/lua
LOCAL_STATIC_LIBRARIES :=$(LUA_LIB)
LOCAL_CFLAGS := -Os -ffunction-sections -fdata-sections
LOCAL_LDFLAGS := $(LOCAL_PATH)/../../../../luaffi/src/main/lua/$(TARGET_ARCH_ABI)/libluadroid.so
LOCAL_MODULE :=ssl
LOCAL_SHARED_LIBRARIES := boringssl boringcrypto
LOCAL_SHORT_COMMANDS := true
$(info local c includes=$(LOCAL_C_INCLUDES))
include $(BUILD_SHARED_LIBRARY)

$(call import-add-path,$(LOCAL_PATH)/../)
$(call import-module, boringssl)