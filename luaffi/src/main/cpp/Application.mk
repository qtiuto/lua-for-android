APP_ABI := arm64-v8a ,armeabi-v7a,x86,x86_64
APP_PLATFORM := android-14
NDK_TOOLCHAIN_VERSION := clang
APP_CPPFLAGS += -std=c++14
 #no exception to support luajit
APP_CPPFLAGS += -fno-exceptions -fno-rtti \

APP_CFLAGS += -std=c11

#APP_CPPFLAGS += -fpermissive