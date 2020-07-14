//
// Created by Vova on 13.07.2020.
//

#pragma once

#include "color.h"


void show_image (const image &img, const std::string &image_name); // Assume image is in RGB, elements are of type double
image open_image(const std::string& filename);                     // Resultant image is in RGB, type is double


void convert_image_between_RGB_and_BGR(image& img);
image convert_image_between_RGB_and_BGR(const image& img);

image convert_from_rgb_to_hsv(const image& img);
image convert_from_hsv_to_rgb(const image& img);


image convert_image_to_floating_point(const image& img); // From uint8_t 3 channels to double
image convert_image_from_floating_point(const image& img); // From double 3 channels to uint8_t
