//
// Created by Vova on 22.07.2021.
//

#pragma once

#include <common_operations/stroke_limits.h>
#include "common_operations/basic_constraining.h"
#include "io_api/image_io_utils.h"
#include "rasterization/stroke_rasterizer.h"
#include "rasterization/drawer.h"


inline void problematic_stroke_constraining_test(const fs::path& image_path)
{
	// fs::path image_path = painter_base_path / "resources" / "";

	auto stroke_json = json::parse(*read_file(painter_base_path / "tests" / "constraining_tests" / "problematic_stroke_to_test_constraining.json"));
	auto problematic_stroke = stroke_json.get<colored_stroke>();

	stroke_limit_descriptor descriptor = {
		.min_dx = 0.,
		.max_dx = 100000.,

		.min_dy = 0.,
		.max_dy = 100000.,

		.min_width = 0.,
		.max_width = 10000000.,

		.image_rectangle = get_image_range_limits<double>(open_image(image_path.string())),
	};

	bool was_already_OK = descriptor.constrain_stroke_to_requirements(problematic_stroke);
	std::cout << std::boolalpha << "Was already OK: " << was_already_OK << std::endl;

	json output_stroke {problematic_stroke};
	// std::cout << output_stroke << std::endl;
	fs::path out_path = painter_base_path / "tests" / "constraining_tests" / "constrained_prob_stroke.json";
	write_file(output_stroke.dump(4), out_path);
}
