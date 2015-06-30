#pragma once

#include <SDL2/SDL.h>
#include <algorithm>

class InputManager
{
public:
    InputManager() 
    {
        std::fill(std::begin(m_oldStates), std::end(m_oldStates), false);
        std::fill(std::begin(m_newStates), std::end(m_newStates), false);
    }
    ~InputManager() {}

    void onKeyDown(SDL_Scancode scancode) { m_newStates[scancode] = true; }
    void onKeyUp(SDL_Scancode scancode) { m_newStates[scancode] = false; }

    bool getKey(SDL_Scancode scancode) const { return m_newStates[scancode]; }
    bool getKeyDown(SDL_Scancode scancode) const { return m_newStates[scancode] && !m_oldStates[scancode]; }
    bool getKeyUp(SDL_Scancode scancode) const { return !m_newStates[scancode] && m_oldStates[scancode]; }

    void update() { std::copy(std::begin(m_newStates), std::end(m_newStates), std::begin(m_oldStates)); }

private:
    bool m_oldStates[SDL_NUM_SCANCODES];
    bool m_newStates[SDL_NUM_SCANCODES];
};