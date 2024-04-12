cmake_minimum_required(VERSION 3.22.1)

project(taglib)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fvisibility=hidden -flto -fdata-sections -ffunction-sections -fomit-frame-pointer")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden -flto -fdata-sections -ffunction-sections -fomit-frame-pointer")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -flto -Wl,--gc-sections -s")

add_subdirectory(taglib)

include_directories(
        taglib/taglib
        taglib/taglib/toolkit
        taglib/taglib/asf
        taglib/taglib/mpeg
        taglib/taglib/ogg
        taglib/taglib/ogg/flac
        taglib/taglib/flac
        taglib/taglib/mpc
        taglib/taglib/mp4
        taglib/taglib/ogg/vorbis
        taglib/taglib/ogg/speex
        taglib/taglib/ogg/opus
        taglib/taglib/mpeg/id3v2
        taglib/taglib/mpeg/id3v2/frames
        taglib/taglib/mpeg/id3v1
        taglib/taglib/ape
        taglib/taglib/wavpack
        taglib/taglib/trueaudio
        taglib/taglib/riff
        taglib/taglib/riff/aiff
        taglib/taglib/riff/wav
        taglib/taglib/mod
        taglib/taglib/s3m
        taglib/taglib/it
        taglib/taglib/xm
        taglib/taglib/dsf
        taglib/taglib/dsdiff
)

add_library(${CMAKE_PROJECT_NAME} SHARED
        taglib.cpp
        fileref_ext.cpp)

target_link_libraries(${CMAKE_PROJECT_NAME}
        android
        tag)
