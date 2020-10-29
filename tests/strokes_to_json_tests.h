//
// Created by Vova on 29.10.2020.
//

#pragma once

#include "data_representation/stroke.h"
#include "common_operations/stroke_generation.h"



inline void test_stroke_to_json() {
	fs::path path = painter_base_path / "tests" / "output" / "strokes_привет.json";

	std::cout << path.string() << std::endl;
	std::wcout << path.wstring() << std::endl;
	// std::cout << recode::to_utf8(path.wstring()) << std::endl;


	size_t w = 500;
	size_t h = 300;

	double width = 30;

	double position_from_factor = 0.1;

	auto stroke_limit = stroke_limit_descriptor {
			double(w) * 0.5,
			double(w) * 0.7,
			double(h) * 0.5,
			double(h) * 0.7,
			width * 0.8,
			width * 1.2,
			{
					double(w) * position_from_factor,
					double(w) * (1 - position_from_factor),
					double(h) * position_from_factor,
					double(h) * (1 - position_from_factor)
			}
	};

	std::vector<stroke> strokes(10);
	std::generate(strokes.begin(), strokes.end(), [&stroke_limit] { return generate_stroke(stroke_limit); });

	json json_strokes = strokes;

	std::string json_to_output = json_strokes.dump(1, '\t');

	write_file(json_to_output, path);
}
