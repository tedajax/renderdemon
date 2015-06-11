#pragma once

#include <cstdint>

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef unsigned int uint;

typedef float f32;
typedef double f64;

struct Point
{
    Point() : x(0), y(0) {}
    Point(int x_, int y_) : x(x_), y(y_) {}
    int x, y;
};