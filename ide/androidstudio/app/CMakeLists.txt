cmake_minimum_required(VERSION 3.4.1)

file(GLOB ui_sources
    ../../../deps/djinni/support-lib/jni/*.cpp
    ../../../src/bridge/jni/*.cpp
    ../../../src/platform/*.cpp
    ../../../src/platform/android/*.cpp
    ../../../src/*.cpp
    ../../../src/openstack/openstack.cpp
)

add_library(cloudstore SHARED ${ui_sources})

# include directories for header files
include_directories(
    ../../../deps/djinni/support-lib/
    ../../../deps/djinni/support-lib/jni/
    ../../../src/bridge/cpp/
    ../../../src/bridge/jni/
    ../../../src/platform
    ../../../src/platform/android
    ../../../
)

target_link_libraries(cloudstore log)

