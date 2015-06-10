#include "Video.h"
#include <cmath>

Video::Video(int width, int height, SDL_Renderer* renderer)
    : m_width(width),
    m_height(height),
    m_renderer(renderer)
{
    m_surface = SDL_CreateRGBSurface(0, width, height, 32,
        0x000000FF,
        0x0000FF00,
        0x00FF0000,
        0x00000000);
}

Video::~Video()
{
    SDL_FreeSurface(m_surface);
}

void Video::setDrawColor(uint8 r, uint8 g, uint8 b)
{
    m_drawColor.r = r;
    m_drawColor.g = g;
    m_drawColor.b = b;
}

void Video::setClearColor(uint8 r, uint8 g, uint8 b)
{
    m_drawColor.r = r;
    m_drawColor.g = g;
    m_drawColor.b = b;
}

void Video::clear()
{
    SDL_SetRenderDrawColor(m_renderer, m_clearColor.r, m_clearColor.g, m_clearColor.b, 255);
    SDL_RenderClear(m_renderer);
}

void Video::present()
{
    SDL_Texture* texture = SDL_CreateTextureFromSurface(m_renderer, m_surface);
    SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 255);
    SDL_RenderCopy(m_renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(m_renderer);
}

uint32* Video::getPixel(int x, int y)
{
    if (x < 0 || x >= m_width || y < 0 || y >= m_height)
    {
        return nullptr;
    }

    int bpp = m_surface->format->BytesPerPixel;
    return (uint32*)((uint8*)m_surface->pixels + y * m_surface->pitch + x * bpp);
}

void Video::setPixel(uint32* p)
{
    *p = m_drawColor.r + (m_drawColor.g << 8) + (m_drawColor.b << 16);
}

void Video::pointc(int x, int y, int count)
{
    uint32* p = getPixel(x, y);
    if (!p) { return; }

    int pos = y * m_surface->w + x;
    int c = count;
    if (pos + c >= m_surface->w * m_surface->h)
    {
        c = m_surface->w * m_surface->h;
    }
    for (int i = 0; i < c; ++i)
    {
        setPixel(p++);
    }
}

void Video::point(int x, int y)
{
    uint32* p = getPixel(x, y);
    if (p)
    {
        setPixel(p);
    }
}

void Video::points(int* data, int count)
{
    for (int i = 0; i < count; ++i)
    {
        point(data[i * 2 + 0], data[i * 2 + 1]);
    }
}

void Video::vline(int x, int y1, int y2)
{
    for (int y = y1; y <= y2; ++y)
    {
        point(x, y);
    }
}

void Video::hline(int y, int x1, int x2)
{
    int right = x2;
    if (right > m_width) { right = m_width; }
    pointc(x1, y, (right - x1));
}

void Video::line(int x1, int y1, int x2, int y2)
{
    int xdiff = abs(x2 - x1);
    int ydiff = abs(y2 - y1);

    if (xdiff == 0)
    {
        vline(x1, y1, y2);
    }
    else if (ydiff == 0)
    {
        hline(y1, x1, x2);
    }
}

void Video::rect(int x1, int y1, int x2, int y2)
{
    line(x1, y1, x2, y1);
    line(x2, y1, x2, y2);
    line(x1, y1, x1, y2);
    line(x1, y2, x2, y2);
}

void Video::fillRect(int x1, int y1, int x2, int y2)
{
    for (int y = y1; y <= y2; ++y)
    {
        hline(y, x1, x2);
    }
}

void Video::circle(int x, int y, int r, int segments)
{
    ellipse(x, y, r, r, segments);
}

void Video::fillCircle(int x, int y, int r, int segments)
{
    fillEllipse(x, y, r, r, segments);
}

void Video::ellipse(int x, int y, int rx, int ry, int segments)
{
    f32 delta = ((f32)M_PI * 2.f) / (f32)segments;
    f32 angle = 0.f;

    for (int i = 0; i < segments; ++i)
    {
        f32 nextAngle = angle + delta;

        int x1 = (int)(cosf(angle) * rx) + x;
        int y1 = (int)(sinf(angle) * ry) + y;
        int x2 = (int)(cosf(nextAngle) * rx) + x;
        int y2 = (int)(sinf(nextAngle) * ry) + y;
        line(x1, y1, x2, y2);

        angle = nextAngle;
    }
}

void Video::fillEllipse(int x, int y, int rx, int ry, int segments)
{
    int halfry = ry / 2;
    int from = y - halfry;
    int to = y + halfry;

    for (int i = from; i < to; ++i)
    {
        f32 p = f32(i - from) / (f32)ry;
        f32 w = cosf(p * 2) * rx;

        int halfw = w / 2;
        hline(y, -halfw, halfw);
    }
}

void Video::test()
{
    /*setDrawColor(255, 0, 0);
    line(0, 0, 200, 200);*/

    setDrawColor(0, 255, 0);
    point(100, 100);
    point(101, 100);
    point(102, 100);
    point(103, 100);
    point(104, 100);
    point(105, 100);
    point(106, 100);
    point(107, 100);
    point(108, 100);
    rect(50, 50, 150, 75);
    fillRect(50, 10, 40, 30);

    setDrawColor(0, 0, 255);
    circle(300, 200, 50);
    fillCircle(350, 200, 50);
}