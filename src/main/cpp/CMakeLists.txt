cmake_minimum_required(VERSION 3.22.1)

project(taglib)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fvisibility=hidden -flto -fdata-sections -ffunction-sections -fomit-frame-pointer")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden -flto -fdata-sections -ffunction-sections -fomit-frame-pointer")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--exclude-libs,ALL -flto -Wl,--gc-sections -s")

add_subdirectory(taglib)

include_directories(
        taglib/taglib
        taglib/taglib/toolkit)

add_library(${CMAKE_PROJECT_NAME} SHARED
        taglib.cpp)

target_link_libraries(${CMAKE_PROJECT_NAME}
        android
        tag
        utf8cpp)
