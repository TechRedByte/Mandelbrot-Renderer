#include <string_view>
#include <iostream>
#include <charconv>
#include <string>

#include "arbitrary.h"
#include "renderer.h"
#include "cli.h"
#include "ui.h"

enum class CliMode {
    NONE = 0, IMAGE
};

int main(int argc, char* argv[]) {
    if (argc == 1) {
        return render_ui();
    } else {
        CliMode mode = CliMode::NONE;
        Dimensions dimension;
        FractalSettings settings;
        std::string output_name;

        for (int i = 1; i < argc; i++) {
            std::string arg = argv[i];

            if (arg == "--help") {
                std::cout << "Help\n";
                return 0;

            } else if (arg == "--image-output") {
                if (mode != CliMode::NONE) {
                    std::cerr << "Error: An output mode has already been specified.\n";
                    std::cerr << "Run 'mandelbrot --help' for usage information.\n";
                    return 1;
                }

                if ((i + 1) >= argc) {
                    std::cerr << "Error: '--image-output' requires a filename.\n";
                    std::cerr << "Run 'mandelbrot --help' for usage information.\n";
                    return 1;
                }

                mode = CliMode::IMAGE;
                output_name = argv[i + 1];

                if (output_name.empty()) {
                    std::cerr << "Error: '--image-output' requires a filename.\n";
                    std::cerr << "Run 'mandelbrot --help' for usage information.\n";
                    return 1;
                }

                i++;

            } else if (arg == "--max-iterations") {
                if ((i + 1) >= argc) {
                    std::cerr << "Error: '--max-iterations' requires a valid number.\n";
                    std::cerr << "Run 'mandelbrot --help' for usage information.\n";
                    return 1;
                }

                std::string_view value = argv[i + 1];
                unsigned int iterations{};
                auto [end, error] = std::from_chars(value.data(), value.data() + value.size(), iterations);

                if (error != std::errc{} || end != value.data() + value.size()) {
                    std::cerr << "Error: '--max-iterations' requires a valid number.\n";
                    return 1;
                }

                settings.max_iterations = iterations;
                i++;

            } else if (arg == "--width") {
                if ((i + 1) >= argc) {
                    std::cerr << "Error: '--width' requires a valid number.\n";
                    std::cerr << "Run 'mandelbrot --help' for usage information.\n";
                    return 1;
                }

                std::string_view value = argv[i + 1];
                int width{};
                auto [end, error] = std::from_chars(value.data(), value.data() + value.size(), width);

                if (error != std::errc{} || end != value.data() + value.size()) {
                    std::cerr << "Error: '--width' requires a valid number.\n";
                    return 1;
                }

                dimension.width = width;
                i++;

            } else if (arg == "--height") {
                if ((i + 1) >= argc) {
                    std::cerr << "Error: '--height' requires a valid number.\n";
                    std::cerr << "Run 'mandelbrot --help' for usage information.\n";
                    return 1;
                }

                std::string_view value = argv[i + 1];
                int height{};
                auto [end, error] = std::from_chars(value.data(), value.data() + value.size(), height);

                if (error != std::errc{} || end != value.data() + value.size()) {
                    std::cerr << "Error: '--height' requires a valid number.\n";
                    return 1;
                }

                dimension.height = height;
                i++;

            } else if (arg == "--scale") {
                if ((i + 1) >= argc) {
                    std::cerr << "Error: '--scale' requires a valid number.\n";
                    std::cerr << "Run 'mandelbrot --help' for usage information.\n";
                    return 1;
                }

                MPFR scale = argv[i + 1];
                dimension.update_view(scale, dimension.center_x, dimension.center_y);
                i++;

            } else {
                std::cerr << "Error: Unknown option '" << arg << "'.\n";
                std::cerr << "Run 'mandelbrot --help' for usage information.\n";
                return 1;
            }
        }

        switch (mode) {
            case CliMode::NONE: {
                std::cerr << "Error: No output mode and name specified.\n";
                std::cerr << "Run 'mandelbrot --help' for usage information.\n";
                return 1;
            }
            case CliMode::IMAGE: {
                create_image(output_name, dimension, settings);
            }
        }
        return 0;
    }
}