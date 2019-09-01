
# add_definitions(-DPROJ_ROOT="\"${PROJECT_SOURCE_DIR}\"")

set(TINY_INCLUDE 
        "utils.h"
        "tgaimage.h"
        "geometry.h"
        "model.h"
)

set (TINY_SOURCE
        "tgaimage.cpp"
        "geometry.cpp"
        "model.cpp"            
)

add_library(
    tinyrenderer
        ${TINY_INCLUDE}
        ${TINY_SOURCE}
)

add_executable(
    mesh_main 
        ${TINY_INCLUDE}
        "mesh_main.cpp"
)

target_link_libraries(mesh_main tinyrenderer)

add_executable(
    primitives_main
        ${TINY_INCLUDE}
        "primitives_main.cpp"
)

target_link_libraries(primitives_main tinyrenderer)

add_subdirectory("tests")