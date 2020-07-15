//
// Created by Vova on 16.07.2020.
//

#pragma once

#include "painter_pch.h"

#include "utils/image_io_utils.h"
#include "utils/stroke.h"

void rasterize_stroke(Image& target_image, const stroke& stroke, size_t point_number = 500);
void rasterize_strokes(Image& target_image, const std::vector<stroke>& strokes);