#include "model.h"
#include "tinygl.h"
#include <string>
#include <ctime>

#include "paths.h"
#include "window.h"

using namespace Paths;

const auto MODEL = MODELS[0];
const auto PROJ_NO = "03_";

constexpr int width = 800;
constexpr int height = 800;
constexpr int depth = 255;

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
    memset(image.data(), uchar(0), width * height * 3);

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
        color._rgb[2] += 3;
        render_mesh(zbuffer, image, model, light_dir, color);
    });

    std::cout << "Press any key to exit...\n";

    return 0;
}
