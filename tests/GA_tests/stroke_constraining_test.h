//
// Created by Vova on 30.09.2020.
//

#pragma once

#include "common_operations/basic_constraining.h"
#include "io_api/image_io_utils.h"
#include "rasterization/stroke_rasterizer.h"
#include "rasterization/drawer.h"

inline void visualize_stroke_constraining(const stroke& stroke_to_show_constraining, size_t image_w, size_t image_h, const std::string& example_name) {
	colored_stroke colored_stroke { stroke_to_show_constraining, rgb_color{ 0.5, 0.1, 0.3 } };
	auto stroke_to_change = colored_stroke;

	// TODO: have grey bg
	Image before = make_default_image(image_w, image_h);
	Image after = make_default_image(image_w, image_h);

	std::cout << "Allocated images" << std::endl;

	carefully_constrain_stroke_to_fit_into_rect(stroke_to_change, { 0, double(image_w), 0, double(image_h) });

	std::cout << "Constrained strokes" << std::endl;
	std::cout << "The result is: " << stroke_to_change << std::endl;

	rasterize_stroke(before, colored_stroke);
	rasterize_stroke(after, stroke_to_change);

	std::cout << "Rasterized strokes" << std::endl;

	auto this_dir = fs::path(base_path) / "resources" / "constraining_tests" / example_name;
	fs::create_directory(this_dir);
	std::cout << "Created directory" << std::endl;
	save_image(before, (this_dir / "before.png").string());
	save_image(after, (this_dir / "after.png").string());

	std::cout << "Saved images" << std::endl;
}

inline void launch_stroke_constraining_visualizing_test() {
	visualize_stroke_constraining({
			                              { 2000, 1300 },
			                              { 1500, 1000 },
			                              { 1700, 500 },
			                              100
		}, 1920, 1080, "SimplestExample");

	// std::cout << "Gone out of the scope" << std::endl;
}
