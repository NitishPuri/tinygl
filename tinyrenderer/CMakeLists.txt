

include_directories(${PROJECT_SOURCE_DIR}/tinyrenderer/include)

FILE(GLOB TINY_SOURCE src/*.cpp)

add_library(
    tinyrenderer
        ${TINY_INCLUDE}
        ${TINY_SOURCE}
)

add_subdirectory("src/tests")