#ifndef FRAME_HPP
#define FRAME_HPP

#include <iostream>
#include <string>
#include <functional>
#include "glms.hpp"

using Pixel = uint8_t;

class Frame
{
  private:
    Pixel* cursor;
    Pixel* pixels;

  public:
    int w, h, channel;

    Frame(int fw, int fh, int fchannel);
    ~Frame();

    uint32_t total_pixels();
    void to_png(std::string file_name);
    void for_each_pixel(std::function<void(const Frame& frame, Pixel* pixel, uint32_t x, uint32_t y)> function);

    static void set_color(glm::vec4 color, Pixel* pixel, bool linear = true);
};

#endif // FRAME_HPP
