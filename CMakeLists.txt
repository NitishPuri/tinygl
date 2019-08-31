cmake_minimum_required(VERSION 2.6)

project(tinyrenderer)

add_definitions(/W4 -std=c++14)

add_definitions(/WX)

message("CMAKE_CXX_FLAGS_DEBUG is ${CMAKE_CXX_FLAGS_DEBUG}")
message("CMAKE_CXX_FLAGS_RELEASE is ${CMAKE_CXX_FLAGS_RELEASE}")

add_subdirectory("src")
