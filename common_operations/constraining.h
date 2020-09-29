//
// Created by Vova on 29.09.2020.
//

#pragma once

#include "data_representation/range_rectangle.h"
#include "data_representation/stroke.h"


bool fits_into(const stroke& stroke, const RangeRectangle<double>& rectangle);

void shift_stroke (stroke& stroke, const point& shifting_vector);

/// After these operations the stroke will be in the boundary of the rectangle:
void brute_constrain_stroke_to_fit_into_rect(stroke& stroke, const RangeRectangle<double>& rectangle);      // Croped
void carefully_constrain_stroke_to_fit_into_rect(stroke& stroke, const RangeRectangle<double>& rectangle); // Moved

/// Here the rect is scaled to


