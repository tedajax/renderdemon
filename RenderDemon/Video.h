#pragma once

#include "Types.h"

#include <SDL2/SDL.h>

class Video
{
public:
    Video(int width, int height, SDL_Renderer* renderer);
    ~Video();

    void setDrawColor(uint8 r, uint8 g, uint8 b);
    void setClearColor(uint8 r, uint8 g, uint8 b);

    void clear();
    void present();

    void pointc(int x, int y, int count);

    void point(int x, int y);
    void points(int* data, int count); // count is number of points so expects count * 2 ints
    void vline(int x, int y1, int y2);
    void hline(int y, int x1, int x2);
    void line(int x1, int y1, int x2, int y2);
    void rect(int x1, int y1, int x2, int y2);
    void fillRect(int x1, int y1, int x2, int y2);

    void floodFill(int x, int y);

    void test();

private:
    uint32* getPixel(int x, int y);
    void setPixel(uint32* p);
    SDL_Color getPixelColor(int x, int y);
    void floodFillRecur(int x, int y);

private:
    int m_width;
    int m_height;
    SDL_Renderer* m_renderer;
    SDL_Surface* m_surface;
    SDL_Color m_drawColor;
    SDL_Color m_clearColor;
};

inline bool rgbEqual(const SDL_Color& c1, const SDL_Color& c2)
{
    return c1.r == c2.r &&
        c1.g == c2.g &&
        c1.b == c2.b;
}