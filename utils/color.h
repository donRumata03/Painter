//
// Created by Vova on 13.07.2020.
//

#ifndef PAINTER_COLOR_H
#define PAINTER_COLOR_H

#include <ostream>
#include "painter_pch.h"

using image = cv::Mat;
using for_each_Pixel = cv::Point2d;

class BGR_Vec {}; // A tag type
class RGB_Vec {}; // A tag type


template <class T>
inline void hash_combine(std::size_t & seed, const T & v)
{
	std::hash<T> hasher;
	seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}


/**
 * Color class with rgb color model.
 * r, g, b are from 0 to 1 if T is floating point type
 *
 * For T == uint8_t there are 256 * 256 * 256 possible values, which is 16_777_216 (≈ 17 millions)
 */
template<class T>
struct rgb_color
{
	T r{}, g{}, b{};

	rgb_color() = default;

	rgb_color(BGR_Vec, const cv::Vec<T, 3>& cv_bgr_color) : r(cv_bgr_color[2]), g(cv_bgr_color[1]), b(cv_bgr_color[0]) {}
	rgb_color(RGB_Vec, const cv::Vec<T, 3>& cv_rgb_color) : r(cv_rgb_color[0]), g(cv_rgb_color[1]), b(cv_rgb_color[2]) {}

	template<class Type> friend std::ostream &operator<< (std::ostream &os, const rgb_color<Type> &rgb_color);

	// auto represent() -> std::add_const_t<decltype(std::tie(r, g, b))> const { return std::tie(r, g, b); }
	auto represent() const {
		auto non_const_this = const_cast<rgb_color<T>*>(this);
		return std::tie(non_const_this->r, non_const_this->g, non_const_this->b);
	}
	template<class Type> friend bool operator == (const rgb_color<Type>&, const rgb_color<Type>&);
};

template < class Type >
bool operator== (const rgb_color<Type> & first, const rgb_color<Type> & second)
{
	// return std::tie(first.r, first.g, first.b) == std::tie(second.r, second.g, second.b);
	return first.represent() == second.represent();
}

template<class T>
std::ostream &operator<< (std::ostream &os, const rgb_color<T> &rgb_color)
{
	os << "RGB_Color { r: " << rgb_color.r << " g: " << rgb_color.g << " b: " << rgb_color.b << " }";
	return os;
}

// To be able to put it into hash tables:
namespace std
{
	template<typename T> struct hash<rgb_color<T>>
{
	inline size_t operator()(const rgb_color<T> & col) const
	{
		size_t seed = 0;
		::hash_combine(seed, col.r);
		::hash_combine(seed, col.g);
		::hash_combine(seed, col.b);
		// ::hash_combine(seed, v.first);
		// ::hash_combine(seed, v.second);
		return seed;
	}
};
}


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
// using color = rgba_color;
using Pixel = cv::Point3_<double>;

using byte_color = rgb_color<uint8_t>;
using byte_Pixel = cv::Point3_<uint8_t>;




std::unordered_set<rgb_color<uint8_t>> get_unique_colors(const image& img);
std::vector<byte_color> get_image_color_list();
std::vector<rgb_color<uint8_t>> get_color_top_top(const std::unordered_set<rgb_color<uint8_t>>& color_top, size_t amount);

std::vector<rgb_color<uint8_t>> dummy_get_compressed_unique_colors(const image& img, size_t amount);

std::unordered_set<byte_color> iterative_get_compressed_unique_colors(const image& img, size_t amount);

// TODO: Use 3d distribution and binary search to detect color segments!!!


#endif //PAINTER_COLOR_H
