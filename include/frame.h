#ifndef FRAME_H
#define FRAME_H

#include <cglm/cglm.h>
#include <stb/stb_image.h>
#include <stb/stb_image_write.h>

typedef struct Frame
{
    // always starting at top left
    uint32_t w;
    uint32_t h;
    uint32_t total_pixels;
    uint32_t channels;

    uint8_t* pixels;
    uint8_t* cursor;
} Frame;

typedef void(frame_for_each_func)(Frame* frame, uint8_t* pixel);

Frame* create_frame(uint32_t fw, uint32_t fh, uint32_t fchannel);
void free_frame(Frame* frame);
void frame_to_png(Frame* frame, const char* name);

void color_to_pixel(vec4 color, uint8_t* pixel, int linear_color);
void frame_set_pixel(Frame* frame, vec4 color);
void frame_for_each_pixel(Frame* frame, frame_for_each_func func);

#endif // FRAME_H
