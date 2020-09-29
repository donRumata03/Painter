//
// Created by Vova on 29.09.2020.
//

#pragma once

#include "data_representation/range_rectangle.h"
#include "data_representation/stroke.h"

/// After these operations the stroke will be in the boundary of the rectangle:
void brute_constrain_stroke_to_fit_into_rect(stroke& stroke, const RangeRectangle<double>& rectangle);
void carefully_constrain_stroke_to_fit_into_rect(stroke& stroke, const RangeRectangle<double>& rectangle);

/// Here the rect is


