#ifndef GLMS_HPP
#define GLMS_HPP

#define GLM_FORCE_INLINE
#define GLM_FORCE_XYZW_ONLY

#include <string>
#include <random>

#include <fmt/format.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

inline std::string str_vec3(glm::vec3&& vector)
{
    return fmt::format("{:3} {:3} {:3}", vector.x, vector.y, vector.z);
}

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

#endif // GLMS_HPP
