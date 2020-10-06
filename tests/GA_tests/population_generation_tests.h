//
// Created by Vova on 04.10.2020.
//

#pragma once

#include "painter_pch.h"
#include "rasterization/stroke_rasterizer.h"
#include "GA_operations/stroke_range_generator.h"
#include "GA_operations/util.h"

#include "GA_operations/population_generator.h"

inline void test_stroke_generation() {
	stroke_limit_descriptor limits {
		.min_dx = 1.,
		.max_dx = 5.,

		.min_dy = 1.,
		.max_dy = 5.,

		.min_width = 0.5,
		.max_width = 2,

		.image_rectangle = {
				0., 10., 0., 10.
		}
	};

	std::cout << generate_stroke(limits) << std::endl;
}

inline void test_stroke_generation_with_rasterization() {
//	auto GA_generated_population = GA::generate_population(
//			generate_point_ranges_for_stroke_genome(200, {1920, 1080}, { 5, 20 }), 10)[0];
//	auto GA_strokes = unpack_stroke_data_buffer(GA_generated_population);


	stroke_limit_descriptor limits {
			.min_dx = 20.,
			.max_dx = 100.,

			.min_dy = 20.,
			.max_dy = 100.,

			.min_width = 5,
			.max_width = 10,

			.image_rectangle = {
					0., 1920., 0., 1080.
			}
	};

	final_generator generator(limits, 2000);
	// std::cout << "Prepared Strokes" << std::endl;
	// std::cout << generator.operator()({}, 1) << std::endl;
	std::vector<double> raw_strokes = generator.operator()({}, 1)[0];
	std::vector<colored_stroke> strokes = unpack_stroke_data_buffer(raw_strokes);


	// for (auto& stroke : strokes) std::cout << stroke << std::endl;

	rgb_color stroke_color = { 1., 1., 1. };
	for (auto& stroke : strokes) stroke.background_color = stroke_color;

	Image image = make_default_image(1920, 1080);
	rasterize_strokes(image, strokes);

	show_image_in_system_viewer(image);
}
