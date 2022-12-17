#include "frame.hpp"

#include <future>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

#include "glms.hpp"
USING_GLM;

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

void Frame::for_each_pixel(PixelFunction function, bool single_threaded)
{

    if (single_threaded)
    {
        for (int i = 0; i < 5; i++)
        {
            for (int y = (i + 1) * h / 5 - 1; y >= i * h / 5; y--)
            {
                for (int x = 0; x < w; x++)
                {
                    function(this, (pixels + channel * x) + y * w * channel, x, y);
                }
            }
        }
    }
    else
    {
        auto cal = [&](int i, int sep)
        {
            for (int y = (i + 1) * h / sep - 1; y >= i * h / sep; y--)
            {
                for (int x = 0; x < w; x++)
                {
                    function(this, (pixels + channel * x) + y * w * channel, x, y);
                }
            }
        };

        std::future<void> line_a = std::async(cal, 0, 10);
        std::future<void> line_b = std::async(cal, 1, 10);
        std::future<void> line_c = std::async(cal, 2, 10);
        std::future<void> line_d = std::async(cal, 3, 10);
        std::future<void> line_e = std::async(cal, 4, 10);

        std::future<void> line_f = std::async(cal, 5, 10);
        std::future<void> line_g = std::async(cal, 6, 10);
        std::future<void> line_h = std::async(cal, 7, 10);
        std::future<void> line_i = std::async(cal, 8, 10);
        std::future<void> line_j = std::async(cal, 9, 10);

        line_a.get();
        line_b.get();
        line_c.get();
        line_d.get();
        line_e.get();

        line_f.get();
        line_g.get();
        line_h.get();
        line_i.get();
        line_j.get();
    }
}

void Frame::set_color(vec4 color, Pixel* pixel, bool linear)
{
    float factor = 1.0f;
    if (linear)
    {
        factor = 255.0f;
    }

    color.x = std::clamp(color.x, 0.0f, 1.0f);
    color.y = std::clamp(color.y, 0.0f, 1.0f);
    color.z = std::clamp(color.z, 0.0f, 1.0f);
    color.w = std::clamp(color.w, 0.0f, 1.0f);

    for (int i = 0; i < 3; i++)
    {
        color[i] = pow(color[i], 1.0f / 2.2f);
    }

    for (int i = 0; i < 4; i++)
    {
        pixel[i] = static_cast<Pixel>(factor * color[i]);
    }
}
