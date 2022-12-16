#include <iostream>
#include <string>
#include <vector>

#include "glms.hpp"
#include "tools.hpp"
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

    struct HitRecord
    {
        glm::vec3 point;
        glm::vec3 normal;
        float t;

        bool front_face;

        inline void set_face_normal(const LightRay& r, const glm::vec3 outward_normal)
        {
            front_face = dot(r.dir, outward_normal) < 0;
            normal = front_face ? outward_normal : -outward_normal;
        }
    };

    struct Sphere
    {
        glm::vec3 center;
        float radius;

        Sphere()
        {
        }
        Sphere(glm::vec3 point, float r)
            : center(point),
              radius(r)
        {
        }

        bool hit(LightRay& ray, float t_min, float t_max, HitRecord& record)
        {
            glm::vec3 oc = ray.origin - center;
            auto a = glm::dot(ray.dir, ray.dir);
            auto b = glm::dot(oc, ray.dir);
            float c = glm::dot(oc, oc) - radius * radius;
            float discriminant = b * b - a * c;

            if (discriminant < 0)
            {
                return false;
            }

            float squard = sqrt(discriminant);

            auto root = (-b - squard) / a;
            if (root < t_min || t_max < root)
            {
                root = (-b + squard) / a;
                if (root < t_min || t_max < root)
                    return false;
            }

            record.t = root;
            record.point = ray.at(record.t);
            glm::vec3 outward_normal = (record.point - center) / radius;
            record.set_face_normal(ray, outward_normal);

            return true;
        }
    };

    class Program
    {
      public:
        int main(int argc, char** argv)
        {
            const uint32_t FRAME_WIDTH = 1920;
            const uint32_t FRAME_HEIGHT = 1080;
            const float FRAME_ASPECT = to(float, FRAME_WIDTH) / to(float, FRAME_HEIGHT);

            Frame ff(FRAME_WIDTH, FRAME_HEIGHT, 4);
            float viewport_height = 2.0f;
            float viewport_width = FRAME_ASPECT * viewport_height;
            float focal_length = 1.0f;

            glm::vec3 origin = {0, 0, 0};
            glm::vec3 horizontal = {viewport_width, 0, 0};
            glm::vec3 vertical = {0, viewport_height, 0};
            glm::vec3 lower_left = origin - 0.5f * horizontal - 0.5f * vertical - glm::vec3(0, 0, focal_length);

            Sphere sph({0, 0, -1}, 0.5);
            Sphere sph2({0, -100.5f, -1}, 100);

            Timer frame_timer;

            frame_timer.start();
            ff.for_each_pixel(
                [&](const Frame& frame, Pixel* pixel, uint32_t x, uint32_t y)
                {
                    HitRecord record;

                    float u = to(float, x) / (frame.w - 1);
                    float v = to(float, y) / (frame.h - 1);
                    LightRay r(origin, lower_left + u * horizontal + v * vertical - origin);
                    glm::vec3 direction = glm::normalize(r.dir);
                    float t = 0.5 * (direction.y + 1.0);
                    glm::vec4 color =
                        (1.0f - t) * glm::vec4(1.0, 1.0, 1.0, 1.0) + t * glm::vec4(0.5f, 0.7f, 1.0f, 1.0f);

                    if (sph2.hit(r, 0, 100000, record))
                    {
                        color = {0.5f * (record.normal + glm::vec3(1, 1, 1)), 1.0f};
                    }
                    if (sph.hit(r, 0, 100000, record))
                    {
                        color = {0.5f * (record.normal + glm::vec3(1, 1, 1)), 1.0f};
                    }

                    Frame::set_color(color, pixel);
                });
            frame_timer.finish();

            std::cout << fmt::format("Frame time: {}ms\n", frame_timer.duration_ms);

            ff.to_png("result.png");
            return EXIT_SUCCESS;
        }
    };

} // namespace Program