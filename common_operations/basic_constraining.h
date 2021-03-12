//
// Created by Vova on 29.09.2020.
//

#pragma once

#include "data_representation/range_rectangle.h"
#include "data_representation/stroke.h"


bool fits_into(const stroke& stroke, const RangeRectangle<double>& rectangle);

void shift_stroke (stroke& stroke, const point& shifting_vector);
void scale_stroke (stroke& stroke, double factor);

/// After these operations the stroke will be in the boundary of the rectangle:
void brute_constrain_stroke_to_fit_into_rect(stroke& stroke, const RangeRectangle<double>& rectangle);      // Croped
void carefully_constrain_stroke_to_fit_into_rect(stroke& stroke, const RangeRectangle<double>& rectangle); // Moved

/// Here the rect is scaled to
void constrain_stroke_size_parameters(stroke& stroke, double min_dx, double max_dx, double min_dy, double max_dy, double min_width, double max_width);


