#include <SDL3/SDL.h>
#include <iostream>
#include <cstdint>
#include <vector>
#include <thread>

#include "fractal.h"
#include "global.h"

SDL_Window *window = nullptr;
SDL_Renderer *renderer = nullptr;

static bool _dragging = false;
static int _last_mouse_x;
static int _last_mouse_y;
static bool _got_result = false;
static std::vector<Pixel> _pixels(dimensions.width * dimensions.height);

bool init_sdl(unsigned int width, unsigned int height);

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

            if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    _dragging = true;
                    _last_mouse_x = event.button.x;
                    _last_mouse_y = event.button.y;
                }
            }

            if (event.type == SDL_EVENT_MOUSE_MOTION) {
                if (_dragging) {
                    int mouse_x = event.motion.x;
                    int mouse_y = event.motion.y;
    
                    int mouse_delta_x = mouse_x - _last_mouse_x;
                    int mouse_delta_y = mouse_y - _last_mouse_y;
    
                    dimensions.center_x -= mouse_delta_x * dimensions.scale;
                    dimensions.center_y -= mouse_delta_y * dimensions.scale;
    
                    _last_mouse_x = mouse_x;
                    _last_mouse_y = mouse_y;
                }
            }
            
            if (event.type == SDL_EVENT_MOUSE_BUTTON_UP) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    _dragging = false;
                    assign_task(dimensions);
                }
            }

            if (event.type == SDL_EVENT_MOUSE_WHEEL) {
                if (event.wheel.y > 0) {
                    dimensions.scale *= 0.8;
                } else if (event.wheel.y < 0) {
                    dimensions.scale *= 1.25;
                }

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