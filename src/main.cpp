#include <iostream>
#include <cstdint>
#include <vector>
#include <SDL3/SDL.h>

#include "config.h"
#include "pixel.h"
#include "fractal.h"

int width = 800;
int height = 600;

int main() {
    std::vector<Pixel> pixels(width * height);
    render_fractal(pixels, width, height);

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cout << "SDL_Init failed: " << SDL_GetError() << "\n";
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow(
        "Mandelbrot Renderer",
        width,
        height,
        SDL_WINDOW_RESIZABLE
    );

    if (window == nullptr) {
        std::cout << "SDL_CreateWindow failed: "
                  << SDL_GetError() << "\n";
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, nullptr);

    if (renderer == nullptr) {
        std::cout << "SDL_CreateRenderer failed: " << SDL_GetError() << "\n";
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_Texture *texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGB24,
        SDL_TEXTUREACCESS_STREAMING,
        width,
        height
    );

    SDL_UpdateTexture(
        texture,
        nullptr,
        pixels.data(),
        width * sizeof(Pixel)
    );

    SDL_RenderTexture(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);

    bool running = true;

    while (running) {
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }

            if (event.type == SDL_EVENT_WINDOW_RESIZED) {
                width = event.window.data1;
                height = event.window.data2;

                pixels.resize(width * height);

                render_fractal(pixels, width, height);

                SDL_DestroyTexture(texture);

                texture = SDL_CreateTexture(
                    renderer,
                    SDL_PIXELFORMAT_RGB24,
                    SDL_TEXTUREACCESS_STREAMING,
                    width,
                    height
                );

                SDL_UpdateTexture(
                    texture,
                    nullptr,
                    pixels.data(),
                    width * sizeof(Pixel)
                );
        
                SDL_RenderTexture(renderer, texture, nullptr, nullptr);
                SDL_RenderPresent(renderer);
            }
        }

        SDL_Delay(16);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
