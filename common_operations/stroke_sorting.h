//
// Created by Vova on 06.03.2021.
//

#pragma once

#include <data_representation/color.h>
#include <data_representation/stroke.h>


std::vector<byte_colored_stroke> sort_strokes(const std::vector<byte_colored_stroke>& strokes, double w, double h, size_t zones);

std::vector<std::vector<byte_colored_stroke>> group_sorted_strokes_by_color(const std::vector<byte_colored_stroke>& strokes);
