//
// Created by Vova on 13.07.2020.
//

#ifndef PAINTER_COLOR_H
#define PAINTER_COLOR_H

#include "painter_pch.h"

using image = cv::Mat;
using for_each_Pixel = cv::Point2d;

class BGR_Vec {}; // A tag type
class RGB_Vec {}; // A tag type

/**
 * Color class with rgb color model.
 * r, g, b are from 0 to 1 if T is floating point type
 *
 * For T == uint8_t there are 256 * 256 * 256 possible values, which is 16_777_216 (≈ 17 millions)
 */
template<class T>
struct rgb_color
{
	T r, g, b;

	rgb_color(BGR_Vec, const cv::Vec<T, 3>& cv_bgr_color) : r(cv_bgr_color[2]), g(cv_bgr_color[1]), b(cv_bgr_color[0]) {}
	rgb_color(RGB_Vec, const cv::Vec<T, 3>& cv_rgb_color) : r(cv_rgb_color[0]), g(cv_rgb_color[1]), b(cv_rgb_color[2]) {}
};


/**
 * Color class with rgb color model and alpha channel.
 * r, g, b, a are all from 0 to 1 for T = double
 */
template<class T>
struct rgba_color
{
	T r, g, b, a;
};

using color = rgb_color<double>;
using byte_color = rgb_color<uint8_t>;
// using color = rgba_color;


std::unordered_set<rgb_color<uint8_t>> get_unique_colors(const image& img);
std::vector<byte_color> get_image_color_list();
std::vector<rgb_color<uint8_t>> get_color_top_top(const std::unordered_set<rgb_color<uint8_t>>& color_top, size_t amount);

std::vector<rgb_color<uint8_t>> dummy_get_compressed_unique_colors(const image& img, size_t amount);

std::unordered_set<byte_color> iterative_get_compressed_unique_colors(const image& img, size_t amount);

// TODO: Use 3d distribution and binary search to detect color segments!!!


#endif //PAINTER_COLOR_H
