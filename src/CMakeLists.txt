add_executable(
    tinyrenderer 
        "utils.h"
        "tgaimage.h"
        "tgaimage.cpp"
        "geometry.h"
        "model.h"
        "model.cpp"
        "main.cpp"
)


add_subdirectory("tests")