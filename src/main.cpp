#include <iostream>
#include <string>

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
                return create_image(output_name);
            }
        }

        return 0;
    }
}