#ifndef FRAME_HPP
#define FRAME_HPP

#include <iostream>
#include <string>
#include <functional>
#include "glms.hpp"

class Frame
{
  private:
    uint8_t* cursor;
    uint8_t* pixels;

  public:
    int w, h, channel;

    Frame(int fw, int fh, int fchannel);
    ~Frame();

    void set_pixel(glm::vec4 color, bool linear = true);
    void to_png(std::string file_name);
    void for_each_pixel(std::function<void(Frame* frame, uint8_t*)> function);
};

#endif // FRAME_HPP
