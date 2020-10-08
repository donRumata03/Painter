//
// Created by Vova on 30.09.2020.
//

#pragma once

#include <common_operations/stroke_limits.h>
#include "common_operations/basic_constraining.h"
#include "io_api/image_io_utils.h"
#include "rasterization/stroke_rasterizer.h"
#include "rasterization/drawer.h"

inline void visualize_stroke_constraining(const std::vector<stroke>& strokes_to_show_constraining,
											stroke_limit_descriptor& limit_descriptor,
											const std::string& example_name)
{

	auto image_w = size_t(limit_descriptor.image_rectangle.max_x), image_h = size_t(limit_descriptor.image_rectangle.max_y);

	Image before = make_default_image(image_w, image_h);
	Image after = make_default_image(image_w, image_h);

	std::cout << "Allocated images" << std::endl;

	auto show_constrained_stroke = [&](const stroke& stroke_to_show_constraining){
		colored_stroke colored_stroke { stroke_to_show_constraining, rgb_color{ 0.5, 0.1, 0.3 } };
		auto stroke_to_change = colored_stroke;


		carefully_constrain_stroke_to_fit_into_rect(stroke_to_change, { 0, double(image_w), 0, double(image_h) });
		constrain_stroke_size_parameters(stroke_to_change,
		                                 limit_descriptor.min_dx,
		                                 limit_descriptor.max_dx,
		                                 limit_descriptor.min_dy,
		                                 limit_descriptor.max_dy,

		                                 limit_descriptor.min_width,
		                                 limit_descriptor.max_width);

		std::cout << "Constrained a stroke " << stroke_to_show_constraining << std::endl;
		std::cout << "The result is: " << stroke_to_change << std::endl;

		rasterize_stroke(before, colored_stroke);
		rasterize_stroke(after, stroke_to_change);
	};

	for (const auto& stroke : strokes_to_show_constraining) show_constrained_stroke(stroke);

	auto this_dir = fs::path(base_path) / "resources" / "constraining_tests" / example_name;
	fs::create_directory(this_dir);

	save_image(before, (this_dir / "before.png").string());
	save_image(after, (this_dir / "after.png").string());
}

inline void launch_stroke_constraining_visualizing_test() {
	stroke stroke1 = {
			{ 2000, 1300 },
			{ 1500, 1000 },
			{ 1700, 500 },
			100
	};

	stroke stroke2 = {
			{ -100, -200 },
			{ 1500, 1000 },
			{ 1700, 500 },
			100
	};

	stroke stroke3 = {
			{ 800, 200 },
			{ 1500, 400 },
			{ 600, 500 },
			100
	};

	stroke_limit_descriptor limiter = {
			.min_dx = 200,
			.max_dx = 600,
			.min_dy = 100,
			.max_dy = 300,

			.min_width = 30,
			.max_width = 70,

			.image_rectangle = {
					0., 1920., 0., 1080.
			}
	};

	std::vector<stroke> strokes = { stroke1, stroke2, stroke3 };

	visualize_stroke_constraining(strokes, limiter, "MultipleStrokes");

	// std::cout << "Gone out of the scope" << std::endl;
}
