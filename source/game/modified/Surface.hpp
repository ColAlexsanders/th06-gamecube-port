// Surface.hpp
#pragma once
#include "inttypes.hpp"

struct Surface
{
    u32 width;
    u32 height;
    u32 pitch;          // bytes per row
    u8 bytesPerPixel;
    void *pixels;
};

struct Rect
{
    i32 x, y, w, h;
};
