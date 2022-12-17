#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <memory>
#include <vector>

#include "glms.hpp"
#include "frame.hpp"
#include "tools.hpp"

struct Material;

struct Ray
{
    glm::vec3 origin_;
    glm::vec3 direction_;

    Ray(glm::vec3 origin, glm::vec3 direction)
        : origin_(origin),
          direction_(direction)
    {
    }

    glm::vec3 at(float t) const
    {
        return origin_ + t * direction_;
    }
};

struct HitRecord
{
    float t_;
    glm::vec3 point_;
    glm::vec3 normal_;
    std::shared_ptr<Material> mat_;
    bool front_face_;

    void set_face_normal(const Ray& r, const glm::vec3& outward_normal)
    {
        front_face_ = dot(r.direction_, outward_normal) < 0;
        normal_ = front_face_ ? outward_normal : -outward_normal;
    }
};

struct Material
{
    virtual bool scatter(const Ray& r_in, const HitRecord& rec, glm::vec3& attenuation, Ray& scattered) const = 0;
};

struct lambertian : Material
{
    glm::vec3 albedo;

    lambertian(glm::vec3 color)
        : albedo(color)
    {
    }

    bool scatter(const Ray& r_in, const HitRecord& record, glm::vec3& attenuation, Ray& scattered) const override
    {
        glm::vec3 scatter_direction = record.normal_ + glm::normalize(get_random_in_unit_sphere());

        if (vec3_near_zero(scatter_direction, 1e-8f))
        {
            scatter_direction = record.normal_;
        }

        scattered = Ray(record.point_, scatter_direction);
        attenuation = albedo;
        return true;
    }
};

struct Metal : Material
{
    glm::vec3 albedo;
    float fuzz_;

    Metal(glm::vec3 color, float fuzz)
        : albedo(color),
          fuzz_(fuzz)
    {
    }

    bool scatter(const Ray& r_in, const HitRecord& record, glm::vec3& attenuation, Ray& scattered) const override
    {
        glm::vec3 reflected = glm::reflect(glm::normalize(r_in.direction_), record.normal_);

        scattered = Ray(record.point_, reflected + fuzz_ * get_random_in_unit_sphere());
        attenuation = albedo;
        return glm::dot(scattered.direction_, record.normal_) > 0.0f;
    }
};

struct HitObj
{
    virtual bool hit(const Ray& r, float min_t, float max_t, HitRecord& record) = 0;
};

struct Sphere : HitObj
{
    glm::vec3 center_;
    float radius_;
    std::shared_ptr<Material> mat_;

    Sphere(glm::vec3 center, float radius, std::shared_ptr<Material> mat)
        : center_(center),
          radius_(radius),
          mat_(mat)
    {
    }

    bool hit(const Ray& r, float min_t, float max_t, HitRecord& record) override
    {
        glm::vec3 oc = r.origin_ - center_;
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
        record.mat_ = mat_;
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
    glm::vec3 origin_;

    glm::vec3 horizontal_;
    glm::vec3 vertical_;
    glm::vec3 lower_left_;

    Camera(glm::vec3 origin, float view_w, float view_h, float view_focal)
        : origin_(origin),
          view_w_(view_w),
          view_h_(view_h),
          view_focal_(view_focal)
    {
        horizontal_ = {view_w_, 0, 0};
        vertical_ = {0, view_h_, 0};
        lower_left_ = origin - 0.5f * vertical_ - 0.5f * horizontal_ - glm::vec3(0, 0, view_focal_);
    }

    Ray get_ray(float x, float y)
    {
        return Ray(origin_, lower_left_ + x * horizontal_ + y * vertical_ - origin_);
    }
};

glm::vec3 cal_ray_color(const Ray& r, HitList& list, float depth)
{
    if (depth <= 0)
    {
        return glm::vec3(0, 0, 0);
    }

    HitRecord record;
    if (list.hit(r, 0.001f, std::numeric_limits<float>::infinity(), record))
    {
        Ray scattered({}, {});
        glm::vec3 attenuation;
        if (record.mat_->scatter(r, record, attenuation, scattered))
        {
            return attenuation * cal_ray_color(scattered, list, depth - 1);
        }
        return glm::vec3(0, 0, 0);
    }

    glm::vec3 direction = glm::normalize(r.direction_);
    float t = 0.5 * (direction.y + 1.0f);
    return (1.0f - t) * glm::vec3(1.0f, 1.0f, 1.0f) + t * glm::vec3(0.5f, 0.7f, 1.0f);
}

glm::vec3 non_recursive_ray_color(const Ray& r, HitList& list, float depth)
{
    Ray tmp_r = r;
    glm::vec3 color(1.0f);

    while (depth > 0)
    {
        HitRecord record;
        if (list.hit(tmp_r, 0.001f, std::numeric_limits<float>::infinity(), record))
        {
            glm::vec3 attenuation;
            if (record.mat_->scatter(tmp_r, record, attenuation, tmp_r))
            {
                color *= attenuation;
                depth--;
            }
            else
            {
                color *= glm::vec3(0.0f);
                break;
            }
        }
        else
        {
            glm::vec3 direction = glm::normalize(r.direction_);
            float t = 0.5 * (direction.y + 1.0f);
            color *= ((1.0f - t) * glm::vec3(1.0f, 1.0f, 1.0f) + t * glm::vec3(0.5f, 0.7f, 1.0f));
            break;
        }
    }

    return color;
}

inline HitList list;
void cal_pixel(Frame* frame, Frame::Pixel* pixel, int x, int y)
{
    static Camera camera({0, 0, 0}, float(frame->w) / float(frame->h) * 2.0f, 2.0f, 1.0f);
    static int samples = 100;
    static float max_depth = 50.0f;

    glm::vec3 color = {0, 0, 0};

    for (int i = 0; i < samples; i++)
    {
        float u = float(x + get_random(-0.5f, 0.5f)) / (frame->w - 1);
        float v = float(y + get_random(-0.5f, 0.5f)) / (frame->h - 1);

        Ray r = camera.get_ray(u, v);
        color += cal_ray_color(r, list, max_depth);
    }

    Frame::set_color(glm::vec4(color / float(samples), 1.0f), pixel);
}

int main(int argc, char** argv)
{
    Frame frame(1920, 1080, 4);

    auto material_ground = std::make_shared<lambertian>(glm::vec3(0.8, 0.8, 0.0));
    auto material_center = std::make_shared<lambertian>(glm::vec3(0.7, 0.3, 0.3));
    auto material_left = std::make_shared<Metal>(glm::vec3(1.0, 1.0, 1.0), 0.0f);
    auto material_right = std::make_shared<Metal>(glm::vec3(0.8, 0.6, 0.2), 1.0f);

    list.objs.push_back(std::make_shared<Sphere>(glm::vec3(0.0, -100.5, -1.0), 100.0, material_ground));
    list.objs.push_back(std::make_shared<Sphere>(glm::vec3(0.0, 0.0, -1.0), 0.5, material_center));
    list.objs.push_back(std::make_shared<Sphere>(glm::vec3(-1.0, 0.0, -1.0), 0.5, material_left));
    list.objs.push_back(std::make_shared<Sphere>(glm::vec3(1.0, 0.0, -1.0), 0.5, material_right));

    Timer timer;
    timer.start();
    frame.for_each_pixel(cal_pixel, 15);
    std::cout << timer.finish().duration_ms << "ms" << std::endl;

    frame.to_png("result.png");
    return 0;
}