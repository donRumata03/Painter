//
// Created by Vova on 18.03.2021.
//

#pragma once

#include <common_operations/stroke_sorting.h>
#include <launching/SvgZoneLauncher.h>

#include <launching/GA_parameter_sets.h>
#include "_test_common_utils.h"


inline void test_stroke_sorting_function(const fs::path& sample_image_path) {
	std::vector<colored_stroke> stroke_list;

	size_t w{}, h{};

	if (ends_with(sample_image_path.string(), ".json")) {
		json j = json::parse(*read_file(sample_image_path));

		stroke_list = j.get<std::vector<colored_stroke>>();

		// TODO: get w, h from json
	}
	else if (ends_with(sample_image_path.string(), ".svg")) {
		/// Get rasterized image:
		Image image = SVG_service(sample_image_path).get_raster_original_image();

		/// Somehow get the strokes:
		using WorkerType = AnnealingWorker;

		SvgZoneLauncher<WorkerType> launcher(
				sample_image_path,
				default_stroking_parameters, get_default_special_params<WorkerType>(),
				Canvas::get_default_canvas(), not CanBeParallelized<WorkerType>::value
		);

		launcher.run();
		std::cout << "Run the optimization!" << std::endl;

		/// Save parameters:
		stroke_list = launcher.get_final_strokes(Units::PX, false);
		w = image.cols;
		h = image.rows;

		std::cout << "Collected the strokes!" << std::endl;
	}
	else {
		throw std::runtime_error("");
	}

	std::vector<byte_colored_stroke> lo_fi_color_strokes; lo_fi_color_strokes.reserve(stroke_list.size());
	for (const auto& hi_fi_stroke : stroke_list) {
		lo_fi_color_strokes.push_back(static_cast<byte_colored_stroke>(hi_fi_stroke));
	}

	for (const auto& stroke : lo_fi_color_strokes) {
		std::cout << stroke.background_color << std::endl;
	}

	std::cout << "Converted the strokes!" << std::endl;


	auto sorted_strokes = sort_strokes(lo_fi_color_strokes, w, h, 4);
	for (const auto& stroke : sorted_strokes) {
		std::cout << stroke.background_color << std::endl;
	}
	std::cout << "Sorted the strokes!" << std::endl;



//	auto hi_fi_strokes = std::vector<std::vector<colored_stroke>>();
//
//	for (const auto& stroke_group : sorted_strokes) {
//		hi_fi_strokes.emplace_back();
//
//		for (const auto& this_stroke : stroke_group) {
//			hi_fi_strokes.back().push_back(this_stroke)
//		}
//	}

	visualize_stroke_painting(sorted_strokes, painter_base_path / "log" / "latest" / "sorted_sequence_visualization", w, h, false);
	std::cout << "Visualized the strokes!" << std::endl;

}

