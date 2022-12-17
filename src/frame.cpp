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

void Frame::for_each_pixel(PixelFunction function, int max_thread)
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

    int curr_max_thread = 1;
    for (int i = 1; i <= max_thread; i++)
    {
        if (h % i == 0)
        {
            curr_max_thread = i;
        }
    }
    std::cout << "Cal culating pixel in " << curr_max_thread << " threads" << std::endl;

    std::vector<std::future<void>> lines;
    for (int i = 0; i < curr_max_thread; i++)
    {
        lines.push_back(std::async(cal, i, curr_max_thread));
    }
    for (int i = 0; i < curr_max_thread; i++)
    {
        lines[i].get();
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

    for (int i = 0; i < 3; i++)
    {
        color[i] = pow(color[i], 1.0f / 2.2f);
    }

    for (int i = 0; i < 4; i++)
    {
        pixel[i] = Pixel(factor * color[i]);
    }
}
