//
// Created by Vova on 17.07.2020.
//

#pragma once

#include <utils/stroke.h>
#include "optimization/error/error_computing.h"
#include "stroke_color_optimizing.h"

struct stroke_image_fitting_params
{
	size_t target_stroke_number;

};

struct stroke_fitting_return_struct
{
	std::vector<colored_stroke>;
};


stroke_fitting_return_struct fit_strokes_to_image() {

}

// TODO: add total optimization!
