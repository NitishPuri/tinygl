

include_directories(${PROJECT_SOURCE_DIR}/tinyrenderer/include)

#find_package(tinyrenderer)
find_library(tinyrenderer tinyrenderer 
    PATHS
        ${PROJECT_SOURCE_DIR}/
)

add_executable(mesh_main "mesh_main.cpp")
target_link_libraries(mesh_main tinyrenderer)

add_executable(primitives_main "primitives_main.cpp")
target_link_libraries(primitives_main tinyrenderer)
