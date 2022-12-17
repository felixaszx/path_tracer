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

float get_random(float min, float max)
{
    static thread_local std::mt19937 generator;
    std::uniform_real_distribution<float> distribution(min, max);
    return distribution(generator);
}

#endif // TOOLS_HPP
