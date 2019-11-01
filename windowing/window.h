#pragma once

#include <GLFW/glfw3.h>
#include "image.h"
#include <iostream>

class Window {

public:
    Window(Image* img);

    int InitWindow();

    template<typename T>
    void Run(T update_callback) {
        while (!glfwWindowShouldClose(window))
        {
            // Update. i.e Update scene and render to the image.
            update_callback();

            // Render the texture to the window.
            RenderOnScreen();
        }


        glfwTerminate();
    }    

private:

    void RenderOnScreen();

    GLFWwindow* window;
    Image* image;
    float border = 0.05f;
};
