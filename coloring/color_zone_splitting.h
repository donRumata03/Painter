//
// Created by vova on 01.08.2021.
//


#pragma once

#include <data_representation/color.h>


/**
 * Color number reduction functions:
 */

std::unordered_set<rgb_color<uint8_t>> get_unique_colors(const Image& img);
std::vector<byte_color> get_image_color_list();
std::vector<rgb_color<uint8_t>> get_color_top_top(const std::unordered_set<rgb_color<uint8_t>>& color_top, size_t amount);

std::vector<rgb_color<uint8_t>> dummy_get_compressed_unique_colors(const Image& img, size_t amount);

std::unordered_set<byte_color> iterative_get_compressed_unique_colors(const Image& img, size_t amount);

// TODO: Use 3d distribution and binary search to detect color segments!!!


