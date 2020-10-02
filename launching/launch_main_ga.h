//
// Created by Vova on 02.10.2020.
//

#pragma once

#include "painter_pch.h"
#include "optimization/error/error_computing.h"

#include "optimization/stroke_color_optimizing.h"

#include "optimization/GA_operations/util.h"
#include "optimization/GA_operations/mutation.h"
#include "optimization/GA_operations/final_constrainer.h"
#include "optimization/GA_operations/final_error_function.h"

void launch_the_GA (const std::string &filename);
