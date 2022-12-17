#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <memory>
#include <vector>

#include "glms.hpp"
#include "frame.hpp"
#include "tools.hpp"

using namespace glm;

struct Ray
{
    vec3 origin_;
    vec3 direction_;

    Ray(vec3 origin, vec3 direction)
        : origin_(origin),
          direction_(direction)
    {
    }

    vec3 at(float t) const
    {
        return origin_ + t * direction_;
    }
};

struct HitRecord
{
    float t_;
    vec3 point_;
    vec3 normal_;
    bool front_face_;

    inline void set_face_normal(const Ray& r, const vec3& outward_normal)
    {
        front_face_ = dot(r.direction_, outward_normal) < 0;
        normal_ = front_face_ ? outward_normal : -outward_normal;
    }
};

struct HitObj
{
    virtual bool hit(const Ray& r, float min_t, float max_t, HitRecord& record) = 0;
};

struct Sphere : HitObj
{
    vec3 center_;
    float radius_;

    Sphere(vec3 center, float radius)
        : center_(center),
          radius_(radius)
    {
    }

    bool hit(const Ray& r, float min_t, float max_t, HitRecord& record) override
    {
        vec3 oc = r.origin_ - center_;
        float a = dot(r.direction_, r.direction_);
        float b = dot(r.direction_, oc);
        float c = dot(oc, oc) - radius_ * radius_;
        float discriminant = b * b - a * c;

        if (discriminant < 0)
        {
            return false;
        }

        float sqrt_dis = sqrt(discriminant);

        auto root = (-b - sqrt_dis) / a;
        if (root < min_t || max_t < root)
        {
            root = (-b + sqrt_dis) / a;
            if (root < min_t || max_t < root)
            {
                return false;
            }
        }

        record.t_ = root;
        record.point_ = r.at(record.t_);
        record.normal_ = (record.point_ - center_) / radius_;
        return true;
    }
};

struct HitList : HitObj
{
    std::vector<std::shared_ptr<HitObj>> objs;

    bool hit(const Ray& r, float min_t, float max_t, HitRecord& record) override
    {
        HitRecord tmp_record;
        bool hit_anything = false;
        float closet = max_t;

        for (int i = 0; i < objs.size(); i++)
        {
            if (objs[i]->hit(r, min_t, closet, tmp_record))
            {
                hit_anything = true;
                closet = tmp_record.t_;
                record = tmp_record;
            }
        }

        return hit_anything;
    }
};

struct Camera
{
    float view_w_;
    float view_h_;
    float view_focal_;
    vec3 origin_;

    vec3 horizontal_;
    vec3 vertical_;
    vec3 lower_left_;

    Camera(vec3 origin, float view_w, float view_h, float view_focal)
        : origin_(origin),
          view_w_(view_w),
          view_h_(view_h),
          view_focal_(view_focal)
    {
        horizontal_ = {view_w_, 0, 0};
        vertical_ = {0, view_h_, 0};
        lower_left_ = origin - 0.5f * vertical_ - 0.5f * horizontal_ - vec3(0, 0, view_focal_);
    }

    Ray get_ray(float x, float y)
    {
        return Ray(origin_, lower_left_ + x * horizontal_ + y * vertical_ - origin_);
    }
};

vec3 cal_ray_color(const Ray& r, HitList& list)
{
    HitRecord record;
    if (list.hit(r, 0, std::numeric_limits<float>::infinity(), record))
    {
        return 0.5f * (record.normal_ + vec3(1, 1, 1));
    }

    vec3 direction = normalize(r.direction_);
    float t = 0.5 * (direction.y + 1.0f);
    return (1.0f - t) * vec3(1.0f, 1.0f, 1.0f) + t * vec3(0.5f, 0.7f, 1.0f);
}

inline HitList list;

void cal_pixel(Frame* frame, Frame::Pixel* pixel, int x, int y)
{
    static Camera camera({0, 0, 0}, float(frame->w) / float(frame->h) * 2.0f, 2.0f, 1.0f);
    static int samples = 10;

    vec3 color;

    for (int i = 0; i < samples; i++)
    {
        float u = float(x + get_random(-0.5f, 0.5f)) / (frame->w - 1);
        float v = float(y + get_random(-0.5f, 0.5f)) / (frame->h - 1);

        Ray r = camera.get_ray(u, v);
        color += cal_ray_color(r, list);
    }

    Frame::set_color(vec4(color / float(samples), 1.0f), pixel);
}

int main(int argc, char** argv)
{
    Frame frame(1920, 1080, 4);

    list.objs.push_back(std::make_shared<Sphere>(vec3(0.0f, 0.0f, -1.0f), 0.5f));
    list.objs.push_back(std::make_shared<Sphere>(vec3(0.0f, -100.5f, -1.0f), 100.0f));

    Timer timer;
    timer.start();
    frame.for_each_pixel(cal_pixel);
    std::cout << timer.finish().duration_ms << "ms" << std::endl;

    frame.to_png("result.png");
    return 0;
}