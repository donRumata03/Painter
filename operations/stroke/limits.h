#pragma once

#include "painter_pch.h"
#include "data/range_rectangle.h"
#include "data/stroke.h"


bool is_fitting(const Stroke& stroke, const RangeRectangle<double>& rectangle);

void shift_stroke(Stroke& stroke, const Point& shifting_vector);

void scale_stroke(Stroke& stroke, double factor);

/**
 * After these operation the stroke will be in the boundary of the rectangle
 */
void fit_stroke_into_rect(Stroke& stroke, const RangeRectangle<double>& rectangle, bool croped = false);

// Here the rect is scaled to
void constrain_stroke_size_parameters(Stroke& stroke, double min_dx, double max_dx, double min_dy, double max_dy,
                                      double min_width, double max_width);






