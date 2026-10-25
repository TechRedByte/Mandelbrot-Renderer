#include <SDL3/SDL.h>
#include <optional>
#include <vector>

#include "renderer.h"
#include "logger.h"
#include "global.h"
#include "ui.h"

static SDL_Window *window = nullptr;
static SDL_Renderer *renderer = nullptr;
static SDL_Texture *texture = nullptr;

static bool dragging = false;
static int last_mouse_x;
static int last_mouse_y;
static Dimensions dimensions;
static FractalSettings settings;
static std::vector<Pixel> pixels(dimensions.width * dimensions.height);

static bool _init_sdl(unsigned int width, unsigned int height);
static void _update_texture();
static void _update_renderer();

int render_ui() {
    if (!_init_sdl(dimensions.width, dimensions.height)) {
        log_error("Failed to initialize SDL: %c", SDL_GetError());
        return 0;
    }
    log_info("SDL initialized.");

    initialize_workers(true);
    log_info("Workers initialized.");

    dimensions.update_precision();
    assign_task(dimensions, settings);

    bool running = true;

    // Main event loop
    while (running) {
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }

            if (event.type == SDL_EVENT_WINDOW_RESIZED) {
                dimensions.width = event.window.data1;
                dimensions.height = event.window.data2;

                pixels.resize(dimensions.width * dimensions.height);

                assign_task(dimensions, settings);
            }

            if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    dragging = true;
                    last_mouse_x = event.button.x;
                    last_mouse_y = event.button.y;
                }
            }

            if (event.type == SDL_EVENT_MOUSE_MOTION) {
                if (dragging) {
                    int mouse_x = event.motion.x;
                    int mouse_y = event.motion.y;

                    MPFR mouse_delta_x = mouse_x - last_mouse_x;
                    MPFR mouse_delta_y = mouse_y - last_mouse_y;

                    dimensions.center_x -= mouse_delta_x * dimensions.scale;
                    dimensions.center_y -= mouse_delta_y * dimensions.scale;

                    last_mouse_x = mouse_x;
                    last_mouse_y = mouse_y;

                    assign_task(dimensions, settings);
                }
            }

            if (event.type == SDL_EVENT_MOUSE_BUTTON_UP) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    dragging = false;
                }
            }

            if (event.type == SDL_EVENT_MOUSE_WHEEL) {
                float zoom_factor;
                if (event.wheel.y > 0) { // Zooming in
                    zoom_factor = 0.8;
                } else if (event.wheel.y < 0) { // Zooming out
                    zoom_factor = 1.25;
                }

                dimensions.center_x = dimensions.center_x + (event.wheel.mouse_x - dimensions.width / 2.0) * dimensions.scale * (1.0 - zoom_factor);
                dimensions.center_y = dimensions.center_y + (event.wheel.mouse_y - dimensions.height / 2.0) * dimensions.scale * (1.0 - zoom_factor);
                dimensions.scale = dimensions.scale * zoom_factor;

                dimensions.update_precision();

                assign_task(dimensions, settings);
            }
        }

        if (auto result = return_result()) {
            if (result->dimensions == dimensions && result->settings == settings) {
                log_debug(
                    "Rendered image at: x: %s, y: %s, scale: %s",
                    dimensions.center_x.get_string().c_str(),
                    dimensions.center_y.get_string().c_str(),
                    dimensions.scale.get_string().c_str()
                );

                pixels = result->pixels;
                _update_texture();
            }
        }

        _update_renderer();
        SDL_Delay(16);
    }

    log_info("Quitting...");
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

static void _update_texture() {
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
        pixels.data(),
        dimensions.width * sizeof(Pixel)
    );
}

static void _update_renderer() {
    SDL_RenderTexture(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}

static bool _init_sdl(unsigned int width, unsigned int height) {
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