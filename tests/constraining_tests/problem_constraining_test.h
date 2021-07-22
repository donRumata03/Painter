//
// Created by Vova on 22.07.2021.
//

#pragma once

#include <common_operations/stroke_limits.h>
#include "common_operations/basic_constraining.h"
#include "io_api/image_io_utils.h"
#include "rasterization/stroke_rasterizer.h"
#include "rasterization/drawer.h"


inline void problematic_stroke_constraining_test()
{
	auto stroke_json = json::parse(*read_file(painter_base_path / "tests" / "constraining_tests" / "problematic_stroke_to_test_constraining.json"));
	auto problematic_stroke = stroke_json.get<colored_stroke>();

	// limits

	json output_stroke {problematic_stroke};
	std::cout << output_stroke << std::endl;
}
