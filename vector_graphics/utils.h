#pragma once

#include <svgdocument.h>
#include "painter_pch.h"
#include "data/color.h"


size_t count_substrings(const std::string& source, const std::string& sub);

cv::Rect2i scale_rect(const cv::Rect2i& rect, double scale_factor);

cv::Mat get_raster_image(const lunasvg::SVGDocument& doc, size_t width = 0, size_t height = 0);

cv::Rect get_region_bounds(const cv::Mat& img);

std::string to_viewbox(const cv::Rect& rect);

cv::Rect from_viewbox(const std::string& vbox);

cv::Rect from_viewbox(const lunasvg::SVGDocument& doc);

Color get_element_color(const lunasvg::SVGElement *elem);

cv::Rect gomothety_bounds(const cv::Rect& bounds, double coeff, double min_padding);

cv::Rect limit_bounds(const cv::Rect& original, const cv::Rect& limit);