#include "Video.h"
#include "Util.h"
#include <cmath>
#include <new>
#include <cassert>

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

    const int cDefaultColorPaletteCount = 16;
    m_defaultColorPalette = new SDL_Color[cDefaultColorPaletteCount];

    m_defaultColorPalette[0x0] = { 128, 128, 128, 255 };
    m_defaultColorPalette[0x1] = { 0, 0, 255, 255 };
    m_defaultColorPalette[0x2] = { 0, 255, 0, 255 };
    m_defaultColorPalette[0x3] = { 0, 255, 255, 255 };

    m_defaultColorPalette[0x4] = { 255, 0, 0, 255 };
    m_defaultColorPalette[0x5] = { 255, 0, 255, 255 };
    m_defaultColorPalette[0x6] = { 255, 255, 0, 255 };
    m_defaultColorPalette[0x7] = { 255, 255, 255, 255 };

    m_defaultColorPalette[0x8] = { 128, 128, 128, 255 };
    m_defaultColorPalette[0x9] = { 0, 0, 255, 255 };
    m_defaultColorPalette[0xA] = { 0, 255, 0, 255 };
    m_defaultColorPalette[0xB] = { 0, 255, 255, 255 };

    m_defaultColorPalette[0xC] = { 255, 0, 0, 255 };
    m_defaultColorPalette[0xD] = { 255, 0, 255, 255 };
    m_defaultColorPalette[0xE] = { 255, 255, 0, 255 };
    m_defaultColorPalette[0xF] = { 255, 255, 255, 255 };

    setColorPalette(m_defaultColorPalette, cDefaultColorPaletteCount);
}

Video::~Video()
{
    SDL_FreeSurface(m_surface);
    delete m_defaultColorPalette;
}

void Video::setColorPalette(SDL_Color* palette, int count)
{
    m_colorPalette = palette;
    m_colorPaletteCount = count;
}

void Video::setDrawColor(uint8 r, uint8 g, uint8 b)
{
    m_drawColor.r = r;
    m_drawColor.g = g;
    m_drawColor.b = b;
}

void Video::setDrawColor(int index)
{
    assert(index >= 0 && index < m_colorPaletteCount);
    m_drawColor = m_colorPalette[index];
}

void Video::setClearColor(uint8 r, uint8 g, uint8 b)
{
    m_clearColor.r = r;
    m_clearColor.g = g;
    m_clearColor.b = b;
}

void Video::setClearColor(int index)
{
    assert(index >= 0 && index < m_colorPaletteCount);
    m_clearColor = m_colorPalette[index];
}

void Video::clear()
{
    SDL_SetRenderDrawColor(m_renderer, m_clearColor.r, m_clearColor.g, m_clearColor.b, 255);
    SDL_RenderClear(m_renderer);

    setDrawColor(m_clearColor.r, m_clearColor.g, m_clearColor.b);
    resetView();
    fillRect(0, 0, m_width, m_height);
    setDrawColor(m_drawColor.r, m_drawColor.g, m_drawColor.b);
}

void Video::present()
{
    SDL_Texture* texture = SDL_CreateTextureFromSurface(m_renderer, m_surface);
    SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 255);
    SDL_RenderCopy(m_renderer, texture, nullptr, nullptr);
    SDL_DestroyTexture(texture);
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
    uint32* p = getPixel(x + m_viewOffsetX, y + m_viewOffsetY);
    if (!p) { return; }

    if (x < 0 || x > m_viewWidth || y < 0 || y > m_viewHeight)
    {
        return;
    }

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
    uint32* p = getPixel(x + m_viewOffsetX, y + m_viewOffsetY);
    if (p)
    {
        if (x < 0 || x > m_viewWidth || y < 0 || y > m_viewHeight)
        {
            return;
        }
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

    if (points[1].y == points[2].y)
    {
        triangleFlatBottom(points);
    }
    else if (points[0].y == points[1].y)
    {
        triangleFlatTop(points);
    }
    else
    {
        int x4 = points[0].x + (int)roundf((f32)(points[1].y - points[0].y) / (f32)(points[2].y - points[0].y) * (f32)(points[2].x - points[0].x));
        int y4 = points[1].y;

        Point top[3] = {
            points[0],
            points[1],
            { x4, y4 },
        };

        Point bottom[3] = {
            points[1],
            { x4, y4 },
            points[2],
        };

        triangleFlatBottom(top);
        triangleFlatTop(bottom);
    }
}

void Video::triangleFlatBottom(Point* points)
{
    int topx = points[0].x;
    int topy = points[0].y;
    int y = points[1].y;
    int leftx = points[1].x;
    int rightx = points[2].x;

    f32 mleft = (f32)(leftx - topx) / (f32)(y - topy);
    f32 mright = (f32)(rightx - topx) / (f32)(y - topy);

    f32 left = (f32)topx;
    f32 right = (f32)topx;

    for (int i = topy; i <= y; ++i)
    {
        hline(i, (int)left, (int)right);
        left += mleft;
        right += mright;
    }
}

void Video::triangleFlatTop(Point* points)
{
    int botx = points[2].x;
    int boty = points[2].y;
    int y = points[0].y;
    int leftx = points[0].x;
    int rightx = points[1].x;

    f32 mleft = (f32)(botx - leftx) / (f32)(boty - y);
    f32 mright = (f32)(botx - rightx) / (f32)(boty - y);

    f32 left = (f32)botx;
    f32 right = (f32)botx;

    for (int i = boty; i > y; --i)
    {
        hline(i, (int)roundf(left), (int)roundf(right));
        left -= mleft;
        right -= mright;
    }
}

void Video::quad(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4)
{
    triangle(x1, y1, x2, y2, x4, y4);
    triangle(x2, y2, x3, y3, x4, y4);
}

void Video::resetView()
{
    m_viewOffsetX = 0;
    m_viewOffsetY = 0;
    m_viewWidth = m_width;
    m_viewHeight = m_height;
}

void Video::view(int x1, int y1, int x2, int y2)
{
    m_viewOffsetX = x1;
    m_viewOffsetY = y1;
    m_viewWidth = x2 - x1;
    m_viewHeight = y2 - y1;

    int nx1 = x1 - m_viewOffsetX, nx2 = x2 - m_viewOffsetX;
    int ny1 = y1 - m_viewOffsetY, ny2 = y2 - m_viewOffsetY;

    line(nx1, ny1, nx2, ny1);
    line(nx2, ny1, nx2, ny2);
    line(nx1, ny1, nx1, ny2);
    line(nx1, ny2, nx2, ny2);
}

void Video::test()
{
    /*setDrawColor(255, 0, 0);
    line(0, 0, 200, 200);*/

    setDrawColor(0, 255, 0);
    point(100, 10);
    rect(5, 5, 25, 35);

    setDrawColor(255, 0, 255);
    quad(5, 50, 30, 55, 70, 90, 10, 105);

    setDrawColor(255, 255, 0);
    line(80, 100, 120, 70);

    setDrawColor(0, 255, 255);
    triangle(50, 5, 30, 20, 100, 80);
}