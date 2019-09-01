

include_directories(${PROJECT_SOURCE_DIR}/tinyrenderer/include)

#find_package(tinyrenderer)
find_library(tinyrenderer tinyrenderer 
    PATHS
        ${PROJECT_SOURCE_DIR}/
)

add_executable(1_primitives "1_primitives.cpp")
target_link_libraries(1_primitives tinyrenderer)

add_executable(2_mesh "2_mesh.cpp")
target_link_libraries(2_mesh tinyrenderer)

add_executable(3_z_buffer "3_z_buffer.cpp")
target_link_libraries(3_z_buffer tinyrenderer)

add_executable(4_texture_mapping "4_texture_mapping.cpp")
target_link_libraries(4_texture_mapping tinyrenderer)

add_executable(5_perspective "5_perspective.cpp")
target_link_libraries(5_perspective tinyrenderer)

add_executable(camera "camera.cpp")
target_link_libraries(camera tinyrenderer)
