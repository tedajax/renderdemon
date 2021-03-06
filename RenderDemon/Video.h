#pragma once

#include "Types.h"

#include <SDL2/SDL.h>

class Video
{
public:
    Video(int width, int height, SDL_Renderer* renderer);
    ~Video();

    void setColorPalette(SDL_Color* palette, int count);

    void setDrawColor(uint8 r, uint8 g, uint8 b);
    void setDrawColor(int index);
    void setClearColor(uint8 r, uint8 g, uint8 b);
    void setClearColor(int index);

    void clear();
    void present();

    void pointc(int x, int y, int count);

    void point(int x, int y);
    void points(int* data, int count); // count is number of points so expects count * 2 ints
    void vline(int x, int y1, int y2);
    void hline(int y, int x1, int x2);
    void line(int x1, int y1, int x2, int y2);
    void lines(int* data, int segments);
    void rect(int x1, int y1, int x2, int y2);
    void fillRect(int x1, int y1, int x2, int y2);
    void triangle(int x1, int y1, int x2, int y2, int x3, int y3);
    void quad(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4);

    void resetView();
    void view(int x1, int y1, int x2, int y2);

    void test();

private:
    uint32* getPixel(int x, int y);
    void setPixel(uint32* p);
    SDL_Color getPixelColor(int x, int y);
    
    // drawing helpers
    void triangleFlatBottom(Point* points);
    void triangleFlatTop(Point* points);

private:
    int m_width;
    int m_height;
    SDL_Renderer* m_renderer;
    SDL_Surface* m_surface;
    SDL_Color m_drawColor;
    SDL_Color m_clearColor;

    SDL_Color* m_defaultColorPalette;
    SDL_Color* m_colorPalette;
    int m_colorPaletteCount;

    int m_viewOffsetX;
    int m_viewOffsetY;
    int m_viewWidth;
    int m_viewHeight;
};

inline bool rgbEqual(const SDL_Color& c1, const SDL_Color& c2)
{
    return c1.r == c2.r &&
        c1.g == c2.g &&
        c1.b == c2.b;
}