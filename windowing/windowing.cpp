
#include <GLFW/glfw3.h>

#include "model.h"
#include "tinygl.h"
#include <string>
#include <ctime>

#include "paths.h"

using namespace Paths;

const auto MODEL = MODELS[0];
const auto PROJ_NO = "03_";

constexpr int width = 800;
constexpr int height = 800;
constexpr int depth = 255;

class Window {

public:
    Window(Image* img) {
        image = img;
        auto res = InitWindow();
        if (res == -1) {
            std::cout << "Could not initialize window.\n";
        }
    }

    int InitWindow() {
        /* Initialize the library */
        if (!glfwInit())
            return -1;

        /* Create a windowed mode window and its OpenGL context */
        window = glfwCreateWindow(width, height, "Hello World", NULL, NULL);
        if (!window) {
            glfwTerminate();
            return -1;
        }

        /* Make the window's context current */
        glfwMakeContextCurrent(window);

        glfwSwapInterval(1);

        glfwSetKeyCallback(window, [](GLFWwindow* w, int key, int, int action, int) {
            if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE) {
                glfwSetWindowShouldClose(w, GL_TRUE);
            }
        });

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

    template<typename T>
    void Run(T update_callback) {
        while (!glfwWindowShouldClose(window))
        {
            // Update things here!!
            update_callback();


            /* Render here */
            glClear(GL_COLOR_BUFFER_BIT);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image->data());

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


        glfwTerminate();
    }


private:
    GLFWwindow* window;
    Image* image;
    float border = 0.05f;
};

auto map_to_screen(Vec3f v) {
    auto x = floor((v.x() + 1.f) * width / 2.f + .5f);
    auto y = floor((v.y() + 1.f) * height / 2.f + .5f);
    auto z = floor((v.z() + 1.f) * depth / 2.f + .5f);
    return Vec3f{ x, y, z };
};

void render_mesh(std::vector<float>& zbuffer, Image& image, const Model& model, const Vec3f& light_dir,
    const Color& color) {
    std::clock_t c_start = std::clock();

    // Clear depth and color buffers!
    memset(&zbuffer[0], 0, sizeof(zbuffer[0]) * zbuffer.size());
    memset(image.data(), Color::uchar(0), width * height * 3);

    for (int f = 0; f < model.nfaces(); f++) {
        const auto &face = model.face(f);

        auto get_vertex = [&](auto vidx) { return model.vert(face[vidx].v_idx); };
        auto get_tex = [&](auto vidx) { return model.tex(face[vidx].t_idx); };

        // light intensity
        Vec3f n = (get_vertex(2) - get_vertex(0)) ^ (get_vertex(1) - get_vertex(0));
        n.normalize();
        float intensity = n * light_dir;

        if (intensity < 0)
            continue;

        std::array<Vec3f, 3> vertices{ map_to_screen(get_vertex(0)),
                                       map_to_screen(get_vertex(1)),
                                      map_to_screen(get_vertex(2)) };

        // Flat shading
        auto color_flat = [intensity, &color](Vec3f) {
            //return Color(char(intensity * 255.));
            return color * intensity;
            // return Colors::White;
        };

        triangle(vertices, zbuffer, image, color_flat);
    }

    std::clock_t c_end = std::clock();
    std::cout << "Elapsed time for rendering :: "
        << 1000.0 * (c_end - c_start) / CLOCKS_PER_SEC << "ms\n";

};


int main() {

    Image image(width, height);
    Model model(GetObjPath(MODEL));

    Window window(&image);

    Vec3f light_dir{ 0, 0, -1 };
    std::vector<float> zbuffer(width * height, std::numeric_limits<float>::min());

    Color color = Colors::Red;

    window.Run([&]() {
        //color._rgb[0] += 1;
        color._rgb[1] += 10;
        //color._rgb[2] += 3;
        render_mesh(zbuffer, image, model, light_dir, color);
    });

    std::cout << "Press any key to exit...\n";

    return 0;
}
