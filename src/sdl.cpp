#include <SDL3/SDL.h>

#include "sdl.h"

SDL_Window *window = nullptr;
SDL_Renderer *renderer = nullptr;

bool init_sdl(unsigned int width, unsigned int height) {
	if (!SDL_Init(SDL_INIT_VIDEO)) {
        return false;
    }

    window = SDL_CreateWindow(
        "Mandelbrot Renderer",
        width,
        height,
        SDL_WINDOW_RESIZABLE
    );

    if (window == nullptr) {
        SDL_Quit();
        return false;
    }

    renderer = SDL_CreateRenderer(window, nullptr);

    if (renderer == nullptr) {
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }

	return true;
}