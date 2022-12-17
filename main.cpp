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

struct Dielectric : Material
{
    float ir_;

    Dielectric(float ir)
        : ir_(ir)
    {
    }

    static float reflectance(float cosine, float ref_idx)
    {
        // Schlick's approximation for reflectance.
        float r0 = (1.0f - ref_idx) / (1.0f + ref_idx);
        r0 = r0 * r0;
        return r0 + (1.0f - r0) * pow((1.0f - cosine), 5.0f);
    }

    bool scatter(const Ray& r_in, const HitRecord& record, glm::vec3& attenuation, Ray& scattered) const override
    {
        attenuation = glm::vec3(1.0f, 1.0f, 1.0f);
        float refraction_ratio = record.front_face_ ? (1.0f / ir_) : ir_;

        glm::vec3 r_dir = glm::normalize(r_in.direction_);
        double cos_theta = fmin(dot(-r_dir, record.normal_), 1.0);
        double sin_theta = sqrt(1.0 - cos_theta * cos_theta);

        bool cannot_refract = refraction_ratio * sin_theta > 1.0;
        glm::vec3 direction;

        if (cannot_refract || reflectance(cos_theta, refraction_ratio) > get_random(0.0f, 1.0f))
        {

            direction = glm::reflect(r_dir, record.normal_);
        }
        else
        {
            direction = glm::refract(r_dir, record.normal_, refraction_ratio);
        }

        scattered = Ray(record.point_, direction);

        return true;
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
        glm::vec3 outward_normal_ = (record.point_ - center_) / radius_;
        record.set_face_normal(r, outward_normal_);
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

    glm::vec3 origin_;
    glm::vec3 horizontal_;
    glm::vec3 vertical_;
    glm::vec3 lower_left_;

    Camera(glm::vec3 eye, glm::vec3 look_at, glm::vec3 up, float height, float fov, float aspect)
    {
        float thera = glm::radians(fov);
        float h = tan(thera / 2.0f);
        float view_height = height * h;
        float view_width = aspect * view_height;

        glm::vec3 w = eye - look_at;
        glm::vec3 u = glm::normalize(glm::cross(up, w));
        glm::vec3 v = glm::normalize(glm::cross(w, u));

        origin_ = eye;
        horizontal_ = u * view_width;
        vertical_ = v * view_height;
        lower_left_ = origin_ - 0.5f * horizontal_ - 0.5f * vertical_ - w;
    }

    Ray get_ray(float x, float y) const
    {
        return Ray(origin_, lower_left_ + x * horizontal_ + y * vertical_ - origin_);
    }
};

glm::vec3 cal_ray_color(const Ray& r, HitList& list, int depth)
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

glm::vec3 non_recursive_ray_color(const Ray& r, HitList& list, int depth)
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
                return glm::vec3(0.0f);
            }
        }
        else
        {
            glm::vec3 direction = glm::normalize(r.direction_);
            float t = 0.5 * (direction.y + 1.0f);
            return color * ((1.0f - t) * glm::vec3(1.0f, 1.0f, 1.0f) + t * glm::vec3(0.5f, 0.7f, 1.0f));
        }
    }

    return color;
}

inline HitList list;
void cal_pixel(Frame* frame, Frame::Pixel* pixel, int x, int y)
{
    static Camera camera({-2, 2, 1}, {0, 0, -1}, {0, 1, 0}, 8.0f, 20.0f, 16.0f / 9.0f);
    static int samples = 100;
    static int max_depth = 50;

    glm::vec3 color = {0, 0, 0};

    for (int i = 0; i < samples; i++)
    {
        float u = float(x + get_random(-0.5f, 0.5f)) / (frame->w - 1);
        float v = float(y + get_random(-0.5f, 0.5f)) / (frame->h - 1);

        Ray r = camera.get_ray(u, v);
        color += non_recursive_ray_color(r, list, max_depth);
    }

    Frame::set_color(glm::vec4(color / float(samples), 1.0f), pixel);
}

int main(int argc, char** argv)
{
    Frame frame(1920, 1080, 4);

    auto material_ground = std::make_shared<Metal>(glm::vec3(0.7, 0.4, 0.7), 0.8f);
    auto material_center = std::make_shared<lambertian>(glm::vec3(0.7, 0.3, 0.3));
    auto material_left = std::make_shared<Dielectric>(1.5f);
    auto material_right = std::make_shared<Metal>(glm::vec3(0.3, 0.3, 0.6), 0.05f);

    list.objs.push_back(std::make_shared<Sphere>(glm::vec3(0.0, -100.5, -1.0), 100.0, material_ground));
    list.objs.push_back(std::make_shared<Sphere>(glm::vec3(0.0, 0.0, -1.0), 0.5, material_center));
    list.objs.push_back(std::make_shared<Sphere>(glm::vec3(-1.01, 0.0, -1.0), 0.5, material_left));
    list.objs.push_back(std::make_shared<Sphere>(glm::vec3(-1.01, 0.0, -1.0), -0.48, material_left));
    list.objs.push_back(std::make_shared<Sphere>(glm::vec3(1.01, 0.0, -1.0), 0.5, material_right));

    Timer timer;
    timer.start();
    frame.for_each_pixel(cal_pixel, 15);
    std::cout << timer.finish().duration_s << "s" << std::endl;

    frame.to_png("result.png");
    return 0;
}