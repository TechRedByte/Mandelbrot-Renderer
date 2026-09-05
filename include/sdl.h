#pragma once

#include <SDL3/SDL.h>

extern SDL_Window *window;
extern SDL_Renderer *renderer;

bool init_sdl(unsigned int width, unsigned int height);