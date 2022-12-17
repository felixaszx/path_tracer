#ifndef TOOLS_HPP
#define TOOLS_HPP

#include <random>
#include <chrono>

#include <fmt/core.h>

#include "glms.hpp"

class Timer
{
  private:
    std::chrono::high_resolution_clock::time_point begin_point;
    std::chrono::high_resolution_clock::time_point end_point;

  public:
    float duration_s;
    unsigned int duration_ms;

    void start()
    {
        begin_point = std::chrono::high_resolution_clock::now();
    }

    Timer& finish()
    {
        end_point = std::chrono::high_resolution_clock::now();
        auto ss = std::chrono::duration_cast<std::chrono::milliseconds>(end_point - begin_point);
        duration_ms = ss.count();
        duration_s = (float)duration_ms / 1000;

        return *this;
    }
};

inline float get_random(float min, float max)
{
    static thread_local std::mt19937 generator;
    std::uniform_real_distribution<float> distribution(min, max);
    return distribution(generator);
}

inline glm::vec3 get_random_vec3(float min, float max)
{
    return glm::vec3(get_random(min, max), get_random(min, max), get_random(min, max));
}

inline glm::vec3 get_random_in_unit_sphere()
{
    while (true)
    {
        glm::vec3 p = get_random_vec3(-1.0f, 1.0f);
        if (glm::dot(p, p) >= 1.0f)
        {
            continue;
        }
        return p;
    }
}

inline glm::vec3 get_random_in_hemisphere(const glm::vec3& normal)
{
    glm::vec3 in_unit_spherer = get_random_in_unit_sphere();
    if (glm::dot(in_unit_spherer, normal) > 0.0f)
    {
        return in_unit_spherer;
    }
    else
    {
        return -in_unit_spherer;
    }
}

inline bool vec3_near_zero(const glm::vec3& vector, float s)
{
    return (fabs(vector.x) < s) && (fabs(vector.y) < s) && (fabs(vector.z) < s);
}

#endif // TOOLS_HPP
