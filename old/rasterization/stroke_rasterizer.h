//
// Created by Vova on 16.07.2020.
//

#pragma once

#include "painter_pch.h"

#include "io_api/image_io_utils.h"
#include "data_representation/stroke.h"

#include <rasterization/RasterizedPainting.h>

void rasterize_stroke(Image& target_image, const colored_stroke& stroke,
					  size_t point_number = 100, StrokeRasterizationAlgorithm algo = StrokeRasterizationAlgorithm::smooth);

void rasterize_stroke (RasterizedPainting& target_painting, const colored_stroke& stroke, li layer_index,
                       size_t point_number = 100, StrokeRasterizationAlgorithm algo = StrokeRasterizationAlgorithm::smooth);

void rasterize_strokes(RasterizedPainting& target_painting, const std::vector<colored_stroke>& strokes, size_t point_number = 100, StrokeRasterizationAlgorithm algo = StrokeRasterizationAlgorithm::smooth);
void rasterize_strokes(Image& target_image, const std::vector<colored_stroke>& strokes, size_t point_number = 100, StrokeRasterizationAlgorithm algo = StrokeRasterizationAlgorithm::smooth);
