

include_directories(${PROJECT_SOURCE_DIR}/tinygl/include)

FILE(GLOB TINY_SOURCE src/*.cpp)

add_library(
    tinygl
        ${TINY_INCLUDE}
        ${TINY_SOURCE}
)

add_subdirectory("src/tests")