//
// Created by Vova on 08.10.2020.
//

#pragma once

#include "common_operations/stroke_generation.h"

inline void generate_stoke_image_example() {
	size_t w = 500;
	size_t h = 300;

	double width = 50;

	auto stroke_limit = stroke_limit_descriptor {
		double(w) * 0.5,
		double(w) * 0.6,
		double(h) * 0.5,
		double(h) * 0.6,
		width * 0.8,
		width * 1.2,
		{
			0., double(w), 0., double(h)
		}
	};

	colored_stroke generated_stroke;
	reinterpret_cast<stroke&>(generated_stroke) = generate_stroke(stroke_limit);
	generated_stroke.background_color = { 0.3, 1, 0.1 };

	Image image = make_default_image(w, h);
	rasterize_stroke(image, generated_stroke);

	fs::path output_path = fs::path{base_path} / "resources" / "generated_strokes" / "one_stroke.png";
	show_image_in_system_viewer(image);
	save_image(image, output_path);
}

