#pragma once

#include "utils/image.h"

size_t painted_pixel_number_black_canvas(const Image& image);
size_t painted_pixel_number(const Image& image, Color canvas_color);

std::vector<ColoredPosition> get_painted_pixels(const Image& image, Color canvas_color);

size_t perimeter_length_pixels(const Image& image);
