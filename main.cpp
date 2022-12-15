#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <fmt/format.h>
#include "glms.hpp"

constexpr int RESULT_W = 1920;
constexpr int RESULT_H = 1080;

int main(int argc, char** argv)
{
    std::fstream ppm_file("result.ppm", std::ios_base::out);
    ppm_file << fmt::format("P3\n {} {}\n 255\n", RESULT_W, RESULT_H);

    for (int i = 0; i < RESULT_W * RESULT_H; i++)
    {
        ppm_file << str_vec3({(int)get_random(255), (int)get_random(255), (int)get_random(255)}) << "\n";
    }

    ppm_file.close();
}