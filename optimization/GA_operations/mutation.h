//
// Created by Vova on 25.09.2020.
//

#pragma once

#include "utils/stroke_limits.h"

/**
 * Callable object for mutation in GA
 */
class mutator
{
	stroke_limit_descriptor limits;

	explicit mutator();
};
