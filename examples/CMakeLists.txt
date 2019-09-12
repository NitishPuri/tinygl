
include_directories(${PROJECT_SOURCE_DIR}/tinyrenderer/include)
find_library(tinyrenderer tinyrenderer 
    PATHS
        ${PROJECT_SOURCE_DIR}/
)



macro(add_example example_name)
	message("example : ${example_name}")
    add_compile_definitions(ROOT_DIR="${PROJECT_SOURCE_DIR}/")
    add_executable(${example_name} "${example_name}.cpp")
    target_link_libraries(${example_name} tinyrenderer)    
endmacro()

add_example(1_primitives)
add_example(2_mesh)
add_example(3_z_buffer)
add_example(4_texture_mapping)
add_example(5_perspective)
add_example(6_camera)
add_example(7_shading)
