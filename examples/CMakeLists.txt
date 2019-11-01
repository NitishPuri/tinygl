
include_directories(${PROJECT_SOURCE_DIR}/tinygl/include)
find_library(tinygl tinygl 
    PATHS
        ${PROJECT_SOURCE_DIR}/
)

include_directories(${PROJECT_SOURCE_DIR}/utils)

macro(add_project project_name)
    message("Adding project : ${project_name}")
    # add_compile_definitions(ROOT_DIR="${PROJECT_SOURCE_DIR}/")
    add_definitions(-DROOT_DIR="${PROJECT_SOURCE_DIR}/")
    add_executable(${project_name} "${project_name}.cpp")
    target_link_libraries(${project_name} tinygl)    
endmacro()

add_project(1_primitives)
add_project(2_mesh)
add_project(3_z_buffer)
add_project(4_texture_mapping)
add_project(5_perspective)
add_project(6_camera)
add_project(7_shading)
add_project(7_shading_all)
add_project(8_normal_mapping)
add_project(9_phong_shading)


add_project(file_conversion)


