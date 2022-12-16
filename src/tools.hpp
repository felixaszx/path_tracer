#ifndef TOOLS_HPP
#define TOOLS_HPP

#include <random>

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

#endif // TOOLS_HPP
