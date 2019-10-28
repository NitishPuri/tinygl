
set(TINYGL_HEADER ${PROJECT_SOURCE_DIR}/tinygl/include)

set(GLFW_HEADER ${PROJECT_SOURCE_DIR}/Dependencies/glfw-3.3.bin.WIN64/include)
find_library(GLFW_LIB glfw3
    PATHS ${PROJECT_SOURCE_DIR}/Dependencies/glfw-3.3.bin.WIN64/lib-vc2017/
)

# set(GLAD_HEADER ${PROJECT_SOURCE_DIR}/Dependencies/Glad/include)

macro(add_example example_name)
  message("example : ${example_name}")
  add_definitions(-DROOT_DIR="${PROJECT_SOURCE_DIR}/")
  add_executable(${example_name} "${example_name}.cpp")
  target_include_directories(${example_name} PRIVATE 
    ${TINYGL_HEADER} 
    ${GLFW_HEADER}
  )
  target_link_libraries(${example_name} 
    ${GLFW_LIB} 
    tinygl
    opengl32.lib
  )
endmacro()

add_example(windowing)


