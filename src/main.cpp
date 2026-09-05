#include <iostream>
#include <cstdint>
#include <vector>

#include "pixel.h"
#include "fractal.h"
#include "sdl.h"

unsigned int width = 800;
unsigned int height = 600;
float scale = 0.005;

int main() {
    if (!init_sdl(width, height)) {
        std::cout << "Failed to initialize SDL.";
        return 0;
    }

    std::vector<Pixel> pixels(width * height);
    render_fractal(pixels, width, height, scale);
    
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

                render_fractal(pixels, width, height, scale);

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
