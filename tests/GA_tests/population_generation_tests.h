//
// Created by Vova on 04.10.2020.
//

#pragma once

#include "painter_pch.h"
#include "rasterization/stroke_rasterizer.h"
#include "optimization/GA_operations/stroke_range_generator.h"
#include "optimization/GA_operations/util.h"

inline void test_stroke_generation_with_rasterization() {
	auto generated_population = GA::generate_population(
			generate_point_ranges_for_stroke_genome(200, {1920, 1080}, { 5, 20 }), 10)[0];

	auto strokes = unpack_stroke_data_buffer(generated_population);

	// for (auto& stroke : strokes) std::cout << stroke << std::endl;

	rgb_color stroke_color = { 1., 1., 1. };
	for (auto& stroke : strokes) stroke.background_color = stroke_color;

	Image image = make_default_image(1920, 1080);
	rasterize_strokes(image, strokes);

	show_image_in_system_viewer(image);
}
