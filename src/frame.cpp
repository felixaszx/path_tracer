#include "frame.hpp"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

Frame::Frame(int fw, int fh, int fchannel)
    : w(fw),
      h(fh),
      channel(fchannel)
{
    pixels = new uint8_t[w * h * channel];
    cursor = pixels;
}

Frame::~Frame()
{
    delete[] pixels;
}

void Frame::set_pixel(glm::vec4 color, bool linear)
{
    float factor = 1.0f;
    if (linear)
    {
        factor = 255.0f;
    }

    for (int i = 0; i < channel; i++)
    {
        cursor[i] = static_cast<uint8_t>(factor * color[i]);
    }
    cursor += channel;
}

void Frame::to_png(std::string file_name)
{
    stbi_write_png(file_name.c_str(), w, h, channel, pixels, w * channel);
}

void Frame::for_each_pixel(std::function<void(Frame* frame, uint8_t*)> function)
{
    for (int i = 0; i < w * h * channel; i += channel)
    {
        function(this, pixels + i);
    }
}
