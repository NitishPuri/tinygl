cmake_minimum_required(VERSION 2.6)

# set(CATCH_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/extern/catch)
# list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/Modules")
#include(CTest)

project(tinygl)

if(MSVC)
    # Visual C++
    add_compile_options(/W4 /std:c++17)
    add_compile_options(/WX)
endif()

if(UNIX AND NOT APPLE)
    # Linux/GCC
    add_compile_options(-std=c++1z)
endif()

message("CMAKE_CXX_FLAGS_DEBUG is ${CMAKE_CXX_FLAGS_DEBUG}")
message("CMAKE_CXX_FLAGS_RELEASE is ${CMAKE_CXX_FLAGS_RELEASE}")

add_subdirectory(tinygl)

add_subdirectory(examples)
add_subdirectory(windowing)


