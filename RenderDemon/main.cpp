#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include <SDL2/SDL.h>
#include "Video.h"
#include "Input.h"

#include <cmath>

struct Vec2
{
    f32 x, y;
};

struct Vec3
{
    f32 x, y, z;
};

f32 cross(f32 x1, f32 y1, f32 x2, f32 y2)
{
    return x1 * y2 - y1 * x2;
}

Vec2 intersect(f32 x1, f32 y1, f32 x2, f32 y2, f32 x3, f32 y3, f32 x4, f32 y4)
{
    f32 x = cross(x1, y1, x2, y2);
    f32 y = cross(x3, y3, x4, y4);
    f32 det = cross(x1 - x2, y1 - y2, x3 - x4, y3 - y4);
    x = cross(x, x1 - x2, y, x3 - x4) / det;
    y = cross(x, y1 - y2, y, y3 - y4) / det;
    return { x, y };
}

struct TestRenderer
{
    void update(const InputManager& input)
    {
        if (input.getKey(SDL_SCANCODE_LEFT))
        {
            angle -= 0.1f;
        }

        if (input.getKey(SDL_SCANCODE_RIGHT))
        {
            angle += 0.1f;
        }

        if (input.getKey(SDL_SCANCODE_UP))
        {
            px += cosf(angle);
            py += sinf(angle);
        }

        if (input.getKey(SDL_SCANCODE_DOWN))
        {
            px -= cosf(angle);
            py -= sinf(angle);
        }

        if (input.getKey(SDL_SCANCODE_A))
        {
            px += sinf(angle);
            py -= cosf(angle);
        }

        if (input.getKey(SDL_SCANCODE_D))
        {
            px -= sinf(angle);
            py += cosf(angle);
        }
    }

    void render(Video* ctx)
    {
        ctx->setDrawColor(1);
        ctx->view(4, 40, 103, 149);

        ctx->setDrawColor(14);
        ctx->line(vx1, vy1, vx2, vy2);
        ctx->setDrawColor(8);
        ctx->line(px, py, cosf(angle) * 5.f + px, sinf(angle) * 5.f + py);
        ctx->setDrawColor(15);
        ctx->point(px, py);

        ctx->setDrawColor(2);
        ctx->view(109, 40, 208, 149);

        f32 tx1 = vx1 - px, ty1 = vy1 - py;
        f32 tx2 = vx2 - px, ty2 = vy2 - py;
        f32 tz1 = tx1 * cosf(angle) + ty1 * sinf(angle);
        f32 tz2 = tx2 * cosf(angle) + ty2 * sinf(angle);
        tx1 = tx1 * sinf(angle) - ty1 * cosf(angle);
        tx2 = tx2 * sinf(angle) - ty2 * cosf(angle);

        ctx->setDrawColor(14);
        ctx->line(50 - tx1, 50 - tz1, 50 - tx2, 50 - tz2);
        ctx->setDrawColor(8);
        ctx->line(50, 50, 50, 45);
        ctx->setDrawColor(15);
        ctx->point(50, 50);

        ctx->setDrawColor(3);
        ctx->view(214, 40, 315, 149);

        if (tz1 > 0 || tz2 > 0)
        {
            Vec2 i1 = intersect(tx1, tz1, tx2, tz2, -0.0001, 0.0001, -20, 5);
            Vec2 i2 = intersect(tx1, tz1, tx2, tz2, 0.0001, 0.0001, 20, 5);
            
            if (tz1 <= 0)
            {
                if (i1.y > 0) { tx1 = i1.x; tz1 = i1.y; }
                else { tx1 = i2.x; tz1 = i2.y; }
            }

            if (tz2 <= 0)
            {
                if (i1.y > 0) { tx2 = i1.x; tz2 = i1.y; }
                else { tx2 = i2.x; tz2 = i2.y; }
            }

            f32 x1 = -tx1 * 16 / tz1, y1a = -50 / tz1, y1b = 50 / tz1;
            f32 x2 = -tx2 * 16 / tz2, y2a = -50 / tz2, y2b = 50 / tz2;

            ctx->setDrawColor(14);
            f32 m = (f32)(y1b - y1a) / (f32)(y2b - y2a);
            for (int i = 50 + x1; i < 50 + x2; ++i)
            {
                f32 n = (f32)(i - (50 + x1)) / (f32)((x1 + 50) - (x2 + 50));
                f32 d = (m / 2.f) * n;
                ctx->vline(i, y1a + 50 - d, y1b + 50 + d);
            }
            //ctx->triangle(50 + x1, 50 + y1a, 50 + x2, 50 + y2a, 50 + x1, 50 + y1b);
            //ctx->triangle(50 + x2, 50 + y2a, 50 + x2, 50 + y2b, 50 + x1, 50 + y1b);
            //ctx->quad(50 + x1, 50 + y1a, 50 + x2, 50 + y2a, 50 + x1, 50 + y1b, 50 + x2, 50 + y2b);
            //ctx->line(50 + x1, 50 + y1a, 50 + x2, 50 + y2a);
            //ctx->line(50 + x1, 50 + y1b, 50 + x2, 50 + y2b);
            //ctx->line(50 + x1, 50 + y1a, 50 + x1, 50 + y1b);
            //ctx->line(50 + x2, 50 + y2a, 50 + x2, 50 + y2b);
        }
    }

    int vx1 = 70, vy1 = 20;
    int vx2 = 70, vy2 = 70;

    f32 px = 50.f, py = 50.f;
    f32 angle = 0.f;
};

int main(int argc, char* argv[])
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    const int cScreenWidth = 320;
    const int cScreenHeight = 240;

    SDL_Window* window = SDL_CreateWindow("RenderDemon", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, cScreenWidth, cScreenHeight, SDL_WINDOW_SHOWN);
    SDL_Renderer* sdlRenderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    
    Video ctx(cScreenWidth, cScreenHeight, sdlRenderer);
    ctx.setClearColor(0, 0, 0);
    ctx.setDrawColor(255, 255, 255);

    InputManager input;

    bool running = true;

    TestRenderer renderer;

    while (running)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                running = false;
            }
            else if (event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
                {
                    running = false;
                }
                if (!event.key.repeat)
                {
                    input.onKeyDown(event.key.keysym.scancode);
                }
            }
            else if (event.type == SDL_KEYUP)
            {
                input.onKeyUp(event.key.keysym.scancode);
            }
        }

        ctx.clear();
        
        renderer.update(input);
        renderer.render(&ctx);

        //SDL_Delay(33);

        ctx.present();

        input.update();
    }

    SDL_DestroyRenderer(sdlRenderer);
    SDL_DestroyWindow(window);

    return 0;
}