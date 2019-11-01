#include "model.h"
#include "tinygl.h"
#include <string>
#include <ctime>

#include "paths.h"
#include "app_utils.h"
#include "shaders.h"
#include "window.h"

using namespace Paths;

const auto MODEL = MODELS[0];
const auto PROJ_NO = "03_";

constexpr int width = 800;
constexpr int height = 800;
constexpr int depth = 255;

class CameraManipulator : public Window::WindowInput {
public:
    CameraManipulator(App::Camera& camera) : _camera(camera) {};

    void handleMouseButton(int button, int action, int ) override
    {
        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            if (action == GLFW_PRESS) {
                is_pressed = true;
            }
            else if (action == GLFW_RELEASE) {
                is_pressed = false;
            }
        }
    }

    void handleMouseMove(float xPos, float yPos) override
    {
        if (is_pressed) {
            auto d = _lastPos - Vec2f{ xPos, yPos };
            _camera.eye = _camera.eye + Vec3f{d.x(), -d.y(), 0.0} * 0.1f;
            _camera.initialize();
        }
        _lastPos = Vec2f{ xPos, yPos };
    }

    void handleMouseScroll(float , float yOffset) override
    {
        _camera.eye = _camera.eye - Vec3f{ 0.0, 0.0, yOffset };
        _camera.initialize();
    }

private:
    App::Camera& _camera;
    bool is_pressed = false;
    Vec2f _lastPos;
};

int main() {

    Image image(width, height);
    Model model(GetObjPath(MODEL));

    auto make_camera = [&](Vec3f eye, auto name) {
        auto center = Vec3f{ 0, 0, 0 };
        auto up = Vec3f{ 0, 1, 0 };
        return App::make_camera(eye, center, up, width, height, depth, name);
    };
    auto camera = make_camera({ 0, 0, 10 }, "main_camera");


    CameraManipulator handler(camera);
    Window window(&image, &handler);

    Vec3f light_dir{ 0, 0, -1 };
    std::vector<float> zbuffer(width * height, std::numeric_limits<float>::min());


    auto make_GouraudShader = [](const App::Camera &camera, const Model *model) {
        auto shader = std::make_unique<Shaders::GouraudShader>();
        shader->model = model;
        shader->u_lightDir = Vec3f(0, 0, -1).normalize();
        shader->u_color = Colors::White;
        shader->u_mvp = camera.ViewportProjectionView;
        return shader;
    };

    auto shader = make_GouraudShader(camera, &model);

    auto render_with_shader = [&]() {
        std::clock_t c_start = std::clock();

        // Clear depth and color buffers!
        memset(&zbuffer[0], 0, sizeof(zbuffer[0]) * zbuffer.size());
        memset(image.data(), uchar(0), width * height * 3);

        // shader->updateCamera !!
        shader->u_mvp = camera.ViewportProjectionView;

        // shader->update uniforms !!

        for (int f = 0; f < model.nfaces(); f++) {
            std::array<Vec3f, 3> screen_coord{
                shader->vertex(f, 0), shader->vertex(f, 1), shader->vertex(f, 2) };

            triangle(screen_coord, zbuffer, image, *shader);
        }

        std::clock_t c_end = std::clock();
        std::cout << "Elapsed time for rendering with shader (" << shader->_name
            << ") :: " << 1000.0 * (c_end - c_start) / CLOCKS_PER_SEC << "ms\n";

    };


    window.Run(render_with_shader);

    std::cout << "Press any key to exit...\n";

    return 0;
}
