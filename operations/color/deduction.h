#pragma once

#include "data/color.h"
#include "utils/image.h"


/**
 * As for now assume that that major color is the brightest among colors, that cover >= image_size * p, p ∈ (0, 1)
 * and are non-background (don't have ALL the corner pixels)
 * @param image
 * @param minimal_allowed_percentage
 * @return
 */
Color find_major_image_color(const Image& image, double minimal_allowed_percentage = 0.03);

/**
 * Goes though image's borders and look at their colors
 * @param image
 * @param max_can_be_non_background_color
 * @return
 */
Color find_image_background_color(const Image& image, double max_can_be_non_background_color = 0.2);