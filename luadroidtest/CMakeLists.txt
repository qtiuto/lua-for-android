cmake_minimum_required ( VERSION 3.4.1 )

add_library(libffitest  SHARED ${CMAKE_CURRENT_SOURCE_DIR}/../luaffi/src/main/cpp/test.c)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-format")
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/../lib/src/main/externalLib/lua)

target_link_libraries(libffitest ${CMAKE_CURRENT_SOURCE_DIR}/../luaffi/src/main/lua/${ANDROID_ABI}/libluadroid.so)