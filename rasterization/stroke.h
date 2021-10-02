#pragma once

#include "data/stroke.h"
#include "utils/image.h"
#include "rasterization/rasterized_paining.h"


void rasterize_stroke(Image& target_image, const ColoredStroke& stroke,
                      size_t point_number = 100,
                      StrokeRasterizationAlgorithm algo = StrokeRasterizationAlgorithm::smooth);


void rasterize_stroke(RasterizedPainting& target_painting, const ColoredStroke& stroke, li layer_index,
                      size_t point_number = 100,
                      StrokeRasterizationAlgorithm algo = StrokeRasterizationAlgorithm::smooth);

void rasterize_strokes(RasterizedPainting& target_painting, const std::vector<ColoredStroke>& strokes,
                       size_t point_number = 100,
                       StrokeRasterizationAlgorithm algo = StrokeRasterizationAlgorithm::smooth);

void rasterize_strokes(Image& target_image, const std::vector<ColoredStroke>& strokes, size_t point_number = 100,
                       StrokeRasterizationAlgorithm algo = StrokeRasterizationAlgorithm::smooth);
