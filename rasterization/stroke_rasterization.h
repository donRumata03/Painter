#pragma once

#include "data/stroke.h"
#include "utils/image.h"
#include "rasterization/layer_tracking/pixel_managers/pixel_layer_tracker.h"
#include "rasterization/layer_tracking/rasterized_paining.h"


/* One stroke on simple image */
void rasterize_stroke(Image& target_image, const ColoredStroke& stroke,
                      size_t point_number = 100,
                      StrokeRasterizationAlgorithm algo = StrokeRasterizationAlgorithm::smooth);

/* Multiple strokes on simple image */
void rasterize_strokes(Image& target_image, const std::vector<ColoredStroke>& strokes, size_t point_number = 100,
                       StrokeRasterizationAlgorithm algo = StrokeRasterizationAlgorithm::smooth);

/* One stroke with layer tracking */
template<typename PixelTracker>
void rasterize_stroke(RasterizedPainting<PixelTracker>& target_painting, const ColoredStroke& stroke, li layer_index,
                      size_t point_number = 100,
                      StrokeRasterizationAlgorithm algo = StrokeRasterizationAlgorithm::smooth);

/* Multiple strokes with layer tracking */
template<typename PixelTracker>
void rasterize_strokes(RasterizedPainting<PixelTracker>& target_painting, const std::vector<ColoredStroke>& strokes,
                       size_t point_number = 100,
                       StrokeRasterizationAlgorithm algo = StrokeRasterizationAlgorithm::smooth);


// Template implementation:
#include "stroke_rasterization.inl.h"
