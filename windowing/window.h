#pragma once

#include <GLFW/glfw3.h>
#include "image.h"
#include <iostream>

class Window {

public:

    class WindowInput {
    public:
        virtual void handleMouseButton(int button, int action, int mods) = 0;
        virtual void handleMouseMove(float xPos, float yPos) = 0;
        virtual void handleMouseScroll(float xOffset, float yOffset) = 0;
    };

    Window(Image* img, WindowInput* input_handler = nullptr);

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

    GLFWwindow* window = nullptr;
    WindowInput* input_handler = nullptr;
    Image* image;
    float border = 0.05f;

};
