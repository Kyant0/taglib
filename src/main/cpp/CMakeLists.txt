cmake_minimum_required(VERSION 3.22.1)

project(taglib)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fvisibility=hidden -flto -fdata-sections -ffunction-sections")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden -flto -fdata-sections -ffunction-sections")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--exclude-libs,ALL -flto -Wl,--gc-sections")

add_subdirectory(taglib)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
        ${taglib_SOURCE_DIR}/taglib
        ${taglib_SOURCE_DIR}/taglib/toolkit)

add_library(${CMAKE_PROJECT_NAME} SHARED
        taglib.cpp)

target_link_libraries(${CMAKE_PROJECT_NAME}
        android
        log
        tag)
