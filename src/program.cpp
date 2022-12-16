#include <iostream>
#include <string>
#include <vector>

#include "glms.hpp"
#include "frame.hpp"

namespace Program
{
    struct LightRay
    {
        glm::vec3 origin;
        glm::vec3 dir;

        LightRay(glm::vec3 point, glm::vec3 direction)
            : origin(point),
              dir(direction)
        {
        }
        LightRay()
        {
        }

        glm::vec3 at(float t)
        {
            return origin + t * dir;
        }
    };

    class Program
    {
      public:
        int main(int argc, char** argv)
        {
            const uint32_t FRAME_WIDTH = 1920;
            const uint32_t FRAME_HEIGHT = 1080;
            const float FRAME_ASPECT = static_cast<float>(FRAME_WIDTH) / static_cast<float>(FRAME_HEIGHT);

            Frame ff(FRAME_WIDTH, FRAME_HEIGHT, 4);
            float viewport_height = 2.0f;
            float viewport_width = FRAME_ASPECT * viewport_height;
            float focal_length = 1.0f;

            glm::vec3 origin = {0, 0, 0};
            glm::vec3 horizontal = {viewport_width, 0, 0};
            glm::vec3 vertical = {0, viewport_height, 0};
            glm::vec3 lower_left = origin - 0.5f * horizontal - 0.5f * vertical - glm::vec3(0, 0, focal_length);

            auto hit_sphere = [](glm::vec3 point, float r, LightRay light) -> float
            {
                glm::vec3 oc = light.origin - point;
                auto a = glm::dot(light.dir, light.dir);
                auto b = glm::dot(oc, light.dir);
                float c = glm::dot(oc, oc) - r * r;
                float discriminant = b * b - a * c;

                if (discriminant < 0)
                {
                    return -1.0f;
                }
                else
                {
                    return (-b - sqrt(discriminant)) / (a);
                }
            };

            ff.for_each_pixel(
                [&](const Frame& frame, Pixel* pixel, uint32_t x, uint32_t y)
                {
                    float u = (float)x / (frame.w - 1);
                    float v = (float)y / (frame.h - 1);
                    LightRay r(origin, lower_left + u * horizontal + v * vertical - origin);
                    glm::vec3 direction = glm::normalize(r.dir);
                    float t = 0.5 * (direction.y + 1.0);
                    glm::vec4 color =
                        (1.0f - t) * glm::vec4(1.0, 1.0, 1.0, 1.0) + t * glm::vec4(0.5f, 0.7f, 1.0f, 1.0f);

                    t = hit_sphere(glm::vec3(0, 0, -1.0f), 0.5, r);
                    if (t > 0)
                    {
                        glm::vec3 N = glm::normalize(r.at(t) - glm::vec3(0, 0, -1));
                        color = glm::vec4(0.5f * glm::vec3(N.x + 1, N.y + 1, N.z + 1), 1.0f);
                    }

                    Frame::set_color(color, pixel);
                });

            ff.to_png("result.png");
            return EXIT_SUCCESS;
        }
    };

} // namespace Program