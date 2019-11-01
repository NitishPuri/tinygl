
set(TINYGL_HEADER ${PROJECT_SOURCE_DIR}/tinygl/include)
set(UTILS ${PROJECT_SOURCE_DIR}/utils)

set(GLFW_HEADER ${PROJECT_SOURCE_DIR}/Dependencies/glfw-3.3.bin.WIN64/include)
find_library(GLFW_LIB glfw3
    PATHS ${PROJECT_SOURCE_DIR}/Dependencies/glfw-3.3.bin.WIN64/lib-vc2017/
)

message("Project : windowing")
add_definitions(-DROOT_DIR="${PROJECT_SOURCE_DIR}/")
add_executable(windowing 
  "windowing.cpp"
  "window.h"
  "window.cpp"
)

target_include_directories(windowing PRIVATE 
  ${TINYGL_HEADER} 
  ${GLFW_HEADER}
  ${UTILS}
)

target_link_libraries(windowing 
  ${GLFW_LIB} 
  tinygl
  opengl32.lib
)
