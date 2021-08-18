//
// Created by Vova on 11.03.2021.
//

#pragma once


#include "io_api/image_io_utils.h"


/// Area:

size_t painted_pixel_number_black_canvas(const Image& image);
size_t painted_pixel_number(const Image& image, color canvas_color);


/// Perimeter:

size_t perimeter_length_pixels(const Image& image);
