#include "Video.h"
#include "Util.h"
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

    setDrawColor(m_clearColor.r, m_clearColor.g, m_clearColor.b);
    fillRect(0, 0, m_width, m_height);
    setDrawColor(m_drawColor.r, m_drawColor.g, m_drawColor.b);
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

SDL_Color Video::getPixelColor(int x, int y)
{
    uint32* p = getPixel(x, y);
    if (!p)
    {
        const SDL_Color cBlack = { 0, 0, 0, 255 };
        return cBlack;
    }

    SDL_Color color = {
        *((uint8*)p + 3),
        *((uint8*)p + 2),
        *((uint8*)p + 1),
        *((uint8*)p + 0),
    };

    return color;
}

void Video::pointc(int x, int y, int count)
{
    uint32* p = getPixel(x, y);
    if (!p) { return; }

    int pos = y * m_surface->w + x;
    int c = count;
    if (pos + c >= m_surface->w * m_surface->h)
    {
        c = (m_surface->w * m_surface->h) - pos;
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
    if (y1 > y2) { Util::Swap(y1, y2); }
    for (int y = y1; y <= y2; ++y)
    {
        point(x, y);
    }
}

void Video::hline(int y, int x1, int x2)
{
    if (x1 > x2) { Util::Swap(x1, x2); }
    int right = x2;
    if (right > m_width) { right = m_width; }
    pointc(x1, y, (right - x1));
}

void Video::line(int x1, int y1, int x2, int y2)
{
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);

    if (dx == 0)
    {
        vline(x1, y1, y2);
        return;
    }
    else if (dy == 0)
    {
        hline(y1, x1, x2);
        return;
    }

    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = (dx > dy ? dx : -dy) / 2, e2;

    while (true)
    {
        point(x1, y1);
        if (x1 == x2 && y1 == y2) break;
        e2 = err;
        if (e2 > -dx) { err -= dy; x1 += sx; }
        if (e2 < dy) { err += dx; y1 += sy; }
    }
}

void Video::lines(int* data, int segments)
{
    for (int i = 0; i < segments; ++i)
    {
        line(data[(i * 2) + 0], data[(i * 2) + 1],
             data[((i + 1) * 2) + 0], data[((i + 1) * 2) + 1]);
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

void Video::triangle(int x1, int y1, int x2, int y2, int x3, int y3)
{
    Point points[3] = {
        { x1, y1 },
        { x2, y2 },
        { x3, y3 },
    };

    Util::ArraySort<Point>(points, 3, [](const Point* p1, const Point* p2){ return p1->y < p2->y; });


}

void Video::quad(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4)
{
    int data[10] = {
        x1, y1,
        x2, y2,
        x3, y3,
        x4, y4,
        x1, y1
    };
    lines(data, 4);

    triangle(x1, y1, x2, y2, x4, y4);
    triangle(x2, y2, x3, y3, x4, y4);
}

void Video::floodFill(int x, int y)
{
    uint32* p = getPixel(x, y);
    if (!p) { return; }

    uint32 target = *p;
    floodFillRecur(x, y, target);
}

void Video::floodFillRecur(int x, int y, uint32 target)
{
    uint32* p = getPixel(x, y);
    if (!p) { return; }

    if (*p != target)
    {
        return;
    }

    uint32 dc = m_drawColor.r + (m_drawColor.g << 8) + (m_drawColor.b << 16);
    if (dc == (target & 0x00FFFFFF))
    {
        return;
    }

    *p = dc;

    floodFillRecur(x - 1, y, target);
    floodFillRecur(x + 1, y, target);
    floodFillRecur(x, y - 1, target);
    floodFillRecur(x, y + 1, target);
}

void Video::test()
{
    /*setDrawColor(255, 0, 0);
    line(0, 0, 200, 200);*/

    setDrawColor(0, 255, 0);
    point(100, 10);
    rect(50, 50, 150, 75);

    setDrawColor(255, 0, 255);
    quad(200, 300, 500, 325, 450, 450, 250, 475);

    setDrawColor(255, 255, 0);
    line(200, 200, 300, 250);

    static f32 angle = 0.f;
    angle += 0.01f;

    int px = 250;
    int py = 250;
    int r = 100;
    int x2 = (int)(cosf(angle) * r) + px;
    int y2 = (int)(sinf(angle) * r) + py;
    line(px, py, x2, y2);

    triangle(px - 50, py, px + 50, py, x2, y2);


  
}