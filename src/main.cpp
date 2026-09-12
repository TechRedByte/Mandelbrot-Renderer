#include <iostream>
#include <cstdint>
#include <vector>
#include <thread>

#include "pixel.h"
#include "fractal.h"
#include "sdl.h"

static bool _got_result = false;
static std::vector<Pixel> _pixels(dimensions.width * dimensions.height);

int main() {
    if (!init_sdl(dimensions.width, dimensions.height)) {
        std::cout << "Failed to initialize SDL.";
        return 0;
    }

    initialize_worker();

    assign_task(dimensions);

    SDL_Texture *texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGB24,
        SDL_TEXTUREACCESS_STREAMING,
        dimensions.width,
        dimensions.height
    );

    SDL_UpdateTexture(
        texture,
        nullptr,
        pixels.data(),
        dimensions.width * sizeof(Pixel)
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
                dimensions.width = event.window.data1;
                dimensions.height = event.window.data2;

                _pixels.resize(dimensions.width * dimensions.height);

                assign_task(dimensions);
            }
        }

        {
        std::lock_guard<std::mutex> lock(mutex);
        if (result_available) {
            _pixels = pixels;
            result_available = false;
            _got_result = true;
        }
        }
        if (_got_result) {
            _got_result = false;
            SDL_DestroyTexture(texture);

            texture = SDL_CreateTexture(
                renderer,
                SDL_PIXELFORMAT_RGB24,
                SDL_TEXTUREACCESS_STREAMING,
                dimensions.width,
                dimensions.height
            );

            SDL_UpdateTexture(
                texture,
                nullptr,
                _pixels.data(),
                dimensions.width * sizeof(Pixel)
            );
        }
        SDL_RenderTexture(renderer, texture, nullptr, nullptr);
        SDL_RenderPresent(renderer);

        SDL_Delay(16);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
