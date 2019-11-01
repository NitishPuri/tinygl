#include "window.h"

Window::Window(Image* img, Window::WindowInput* handler) {
    image = img;
    input_handler = handler;
    auto res = InitWindow();
    if (res == -1) {
        std::cout << "Could not initialize window.\n";
    }
}

int Window::InitWindow() {
    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(image->get_width(), image->get_height(), "Hello World", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glfwSwapInterval(1);

    if (input_handler) {
        glfwSetWindowUserPointer(window, input_handler);
        glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods) {
            WindowInput& data = *(WindowInput*)glfwGetWindowUserPointer(window);
            data.handleMouseButton(button, action, mods);
        });
        glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xPos, double yPos) {
            WindowInput& data = *(WindowInput*)glfwGetWindowUserPointer(window);
            data.handleMouseMove(float(xPos), float(yPos));
        });
        glfwSetScrollCallback(window, [](GLFWwindow* window, double xOffset, double yOffset) {
            WindowInput& data = *(WindowInput*)glfwGetWindowUserPointer(window);
            data.handleMouseScroll(float(xOffset), float(yOffset));
        });
    }
    //else {
        glfwSetKeyCallback(window, [](GLFWwindow* w, int key, int, int action, int) {
            if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE) {
                glfwSetWindowShouldClose(w, GL_TRUE);
            }
        });
    //}


    // Bind textures and stuff
    // Doing that here since we only want to do this once.
    glEnable(GL_TEXTURE_2D);
    glClearColor(1.0f, 0.0f, 0.5f, 1.0f);

    GLuint m_texture;
    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


    return 1;
}

void Window::RenderOnScreen()
{
    /* Render here */
    glClear(GL_COLOR_BUFFER_BIT);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image->get_width(), image->get_height(), 0, GL_RGB, GL_UNSIGNED_BYTE, image->data());

    // Draw a full screen quad with "image" mapped as texture.
    {
        float d = 1 - border;

        glBegin(GL_TRIANGLES);

        glTexCoord2f(0, 0);
        glVertex2f(-d, -d);

        glTexCoord2f(0, 1);
        glVertex2f(-d, d);

        glTexCoord2f(1, 1);
        glVertex2f(d, d);

        glTexCoord2f(1, 1);
        glVertex2f(d, d);

        glTexCoord2f(1, 0);
        glVertex2f(d, -d);

        glTexCoord2f(0, 0);
        glVertex2f(-d, -d);

        glEnd();
    }

    /* Swap front and back buffers */
    glfwSwapBuffers(window);

    /* Poll for and process events */
    glfwPollEvents();

}
