

include_directories(${PROJECT_SOURCE_DIR}/tinyrenderer/include)

#find_package(tinyrenderer)
find_library(tinyrenderer tinyrenderer 
    PATHS
        ${PROJECT_SOURCE_DIR}/
)

add_executable(primitives "primitives.cpp")
target_link_libraries(primitives tinyrenderer)

add_executable(mesh "mesh.cpp")
target_link_libraries(mesh tinyrenderer)

add_executable(texture_mapping "texture_mapping.cpp")
target_link_libraries(texture_mapping tinyrenderer)

add_executable(perspective "perspective.cpp")
target_link_libraries(perspective tinyrenderer)

add_executable(camera "camera.cpp")
target_link_libraries(camera tinyrenderer)
