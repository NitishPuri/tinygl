
include_directories(${PROJECT_SOURCE_DIR}/tinygl/include)
find_library(tinygl tinygl 
    PATHS
        ${PROJECT_SOURCE_DIR}/
)



macro(add_example example_name)
	message("example : ${example_name}")
    add_compile_definitions(ROOT_DIR="${PROJECT_SOURCE_DIR}/")
    add_executable(${example_name} "${example_name}.cpp")
    target_link_libraries(${example_name} tinygl)    
endmacro()

add_example(1_primitives)
add_example(2_mesh)
add_example(3_z_buffer)
add_example(4_texture_mapping)
add_example(5_perspective)
add_example(6_camera)
add_example(7_shading)
add_example(7_shading_all)
add_example(8_normal_mapping)
add_example(9_phong_shading)


add_compile_definitions(ROOT_DIR="${PROJECT_SOURCE_DIR}/")
add_executable(file_conversion "file_conversion.cpp")
target_link_libraries(file_conversion tinygl)    


