#ifndef FRAME_HPP
#define FRAME_HPP

#include <iostream>
#include <string>
#include <functional>
#include "glms.hpp"

class Frame
{
  public:
    typedef uint8_t Pixel;
    typedef void (*PixelFunction)(Frame* frame, Pixel* pixel, int x, int y);

  private:
    Pixel* cursor;
    Pixel* pixels;

  public:
    int w, h, channel;

    Frame(int fw, int fh, int fchannel);
    ~Frame();

    uint32_t total_pixels();
    void to_png(std::string file_name);
    void for_each_pixel(PixelFunction function, int max_thread = 15);

    static void set_color(glm::vec4 color, Pixel* pixel, bool linear = true);
};

#endif // FRAME_HPP
