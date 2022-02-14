//
// Created by Vova on 14.02.2022.
//

#pragma once

#include <rasterization/stroke.h>
#include <data/paint_plan.h>

Image rasterize_paint_plan(const PaintPlan &plan, Color canvas_color = { 1., 1., 1. });
