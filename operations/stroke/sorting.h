#pragma once

#include "data/stroke.h"


size_t get_default_number_of_zones_by_stroke_number(size_t stroke_number);

template <class StrokeType>
std::vector<StrokeType> sort_strokes(const std::vector<StrokeType>& strokes, double w, double h, size_t zones = 0);

template <class StrokeType>
std::vector<std::vector<StrokeType>> group_sorted_strokes_by_color(const std::vector<StrokeType>& strokes);