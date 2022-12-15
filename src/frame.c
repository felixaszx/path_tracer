#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "frame.h"

#include <string.h>

Frame* create_frame(uint32_t fw, uint32_t fh, uint32_t fchannel)
{
    Frame* tmp = malloc(sizeof(Frame));
    tmp->w = fw;
    tmp->h = fh;
    tmp->total_pixels = fw * fh;
    tmp->channels = fchannel;

    tmp->pixels = malloc(fw * fh * fchannel);
    memset(tmp->pixels, 0x0, fw * fh * fchannel);
    tmp->cursor = tmp->pixels;

    return tmp;
}

void free_frame(Frame* frame)
{
    frame->w = 0;
    frame->h = 0;
    frame->channels = 0;
    frame->cursor = NULL;

    free(frame->pixels);
    free(frame);
}

void frame_set_pixel(Frame* frame, vec4 color)
{
    for (int i = 0; i < frame->channels; i++)
    {
        *(frame->cursor + i) = (uint8_t)(255.0f * color[i]);
    }
    frame->cursor += frame->channels;
}

void color_to_pixel(vec4 color, uint8_t* pixel, int linear_color)
{
    float factor = 1.0f;
    if (linear_color)
    {
        factor = 255.0f;
    }

    for (int i = 0; i < 4; i++)
    {
        pixel[i] = (uint8_t)(factor * color[i]);
    }
}

void frame_for_each_pixel(Frame* frame, frame_for_each_func func)
{
    for (int i = 0; i < frame->w * frame->h; i++)
    {
        func(frame, frame->pixels + 4 * i);
    }
}

void frame_to_png(Frame* frame, const char* name)
{
    stbi_write_png(name, frame->w, frame->h, frame->channels, frame->pixels, frame->w * frame->channels);
}