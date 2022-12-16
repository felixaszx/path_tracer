#include "frame.hpp"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

Frame::Frame(int fw, int fh, int fchannel)
    : w(fw),
      h(fh),
      channel(fchannel)
{
    pixels = new Pixel[w * h * channel];
    cursor = pixels;
}

Frame::~Frame()
{
    delete[] pixels;
}

uint32_t Frame::total_pixels()
{
    return w * h;
}

void Frame::to_png(std::string file_name)
{
    stbi_flip_vertically_on_write(true);
    stbi_write_png(file_name.c_str(), w, h, channel, pixels, w * channel);
    stbi_flip_vertically_on_write(false);
}

void Frame::for_each_pixel(std::function<void(const Frame& frame, Pixel* pixel, uint32_t x, uint32_t y)> function)
{
    for (int y = h - 1; y >= 0; y--)
    {
        for (int x = 0; x < w; x++)
        {
            function(*this, (pixels + channel * x) + y * w * channel, x, y);
        }
    }
}

void Frame::set_color(glm::vec4 color, Pixel* pixel, bool linear)
{
    float factor = 1.0f;
    if (linear)
    {
        factor = 255.0f;
    }

    for (int i = 0; i < 4; i++)
    {
        pixel[i] = static_cast<Pixel>(factor * color[i]);
    }
}
