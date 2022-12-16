#ifndef TOOLS_HPP
#define TOOLS_HPP

#include <random>
#include <chrono>

#include <fmt/core.h>

#define to(type, x) static_cast<type>(x)

inline float get_random(float l, float r)
{
    static std::default_random_engine eng;
    std::uniform_real_distribution dist6(l, r);

    return dist6(eng);
}

inline float get_random(float r)
{
    return get_random(0.0f, r);
}

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
    void finish()
    {
        end_point = std::chrono::high_resolution_clock::now();
        auto ss = std::chrono::duration_cast<std::chrono::milliseconds>(end_point - begin_point);
        duration_ms = ss.count();
        duration_s = (float)duration_ms / 1000;
    }
};

#endif // TOOLS_HPP
