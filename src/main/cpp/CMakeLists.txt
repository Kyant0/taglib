cmake_minimum_required(VERSION 3.22.1)

project(taglib)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fvisibility=hidden -flto -fdata-sections -ffunction-sections -fomit-frame-pointer")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden -flto -fdata-sections -ffunction-sections -fomit-frame-pointer")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--exclude-libs,ALL -flto -Wl,--gc-sections -s")

add_subdirectory(taglib)

include_directories(
        ${taglib_SOURCE_DIR}/taglib
        ${taglib_SOURCE_DIR}/taglib/toolkit
        ${taglib_SOURCE_DIR}/taglib/asf
        ${taglib_SOURCE_DIR}/taglib/mpeg
        ${taglib_SOURCE_DIR}/taglib/ogg
        ${taglib_SOURCE_DIR}/taglib/ogg/flac
        ${taglib_SOURCE_DIR}/taglib/flac
        ${taglib_SOURCE_DIR}/taglib/mpc
        ${taglib_SOURCE_DIR}/taglib/mp4
        ${taglib_SOURCE_DIR}/taglib/ogg/vorbis
        ${taglib_SOURCE_DIR}/taglib/ogg/speex
        ${taglib_SOURCE_DIR}/taglib/ogg/opus
        ${taglib_SOURCE_DIR}/taglib/mpeg/id3v2
        ${taglib_SOURCE_DIR}/taglib/mpeg/id3v2/frames
        ${taglib_SOURCE_DIR}/taglib/mpeg/id3v1
        ${taglib_SOURCE_DIR}/taglib/ape
        ${taglib_SOURCE_DIR}/taglib/wavpack
        ${taglib_SOURCE_DIR}/taglib/trueaudio
        ${taglib_SOURCE_DIR}/taglib/riff
        ${taglib_SOURCE_DIR}/taglib/riff/aiff
        ${taglib_SOURCE_DIR}/taglib/riff/wav
        ${taglib_SOURCE_DIR}/taglib/mod
        ${taglib_SOURCE_DIR}/taglib/s3m
        ${taglib_SOURCE_DIR}/taglib/it
        ${taglib_SOURCE_DIR}/taglib/xm
        ${taglib_SOURCE_DIR}/taglib/dsf
        ${taglib_SOURCE_DIR}/taglib/dsdiff
        ${CMAKE_CURRENT_SOURCE_DIR})

add_library(${CMAKE_PROJECT_NAME} SHARED
        taglib_ext.cpp
        taglib.cpp)

target_link_libraries(${CMAKE_PROJECT_NAME}
        android
        log
        tag)
