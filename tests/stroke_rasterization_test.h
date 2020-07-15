//
// Created by Vova on 16.07.2020.
//

#pragma once

#include "rasterization/stroke_rasterizer.h"

inline void test_stroke_rasterizaton(){
	Image image(160, 90, CV_64FC3, {0., 0., 0.});
	rasterize_stroke(image,
			stroke({ 70, 30 }, { 20, 50 }, { 100, 80 },
			10, {1., 1., -1}), 1000);
}

