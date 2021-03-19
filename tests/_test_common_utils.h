//
// Created by Vova on 18.03.2021.
//

#pragma once

#include <data_representation/stroke.h>
#include <common_operations/stroke_sorting.h>
#include <io_api/image_io_utils.h>
#include <rasterization/stroke_rasterizer.h>



inline void visualize_stroke_painting(const std::vector<byte_colored_stroke>& stroke_sequence, const fs::path& target_folder_path, size_t w, size_t h, bool use_informative_names = true, color canvas_color = { 0., 0., 0. })
{
	Image image_for_strokes = make_default_image(w, h, canvas_color);

	auto grouped_strokes = group_sorted_strokes_by_color(stroke_sequence);
	std::cout << grouped_strokes.size() << " color zones registered…" << std::endl;

	size_t total_index = 0;
	for (size_t stroke_group_index = 0; stroke_group_index < grouped_strokes.size(); stroke_group_index++) {
		for (size_t index_in_group = 0; index_in_group < grouped_strokes[stroke_group_index].size(); ++index_in_group) {
			auto& this_stroke = grouped_strokes[stroke_group_index][index_in_group];
			rasterize_stroke(image_for_strokes, static_cast<colored_stroke>(this_stroke));

			std::cout << target_folder_path / ("Color " + std::to_string(stroke_group_index) + ", stroke №" + std::to_string(index_in_group) + " (" + std::to_string(total_index) + "th in global order).png") << "…" << std::endl;
			// show_image_in_system_viewer(image_for_strokes);

			std::string output_filename =
					(use_informative_names ?
			                              target_folder_path / ("Color " + std::to_string(stroke_group_index) + ", stroke " + std::to_string(index_in_group) + " (" + std::to_string(total_index) + "th in global order).png")
			                                                    :
			                              target_folder_path / (std::to_string(total_index) + ".png")
					).string();

			save_image(
					image_for_strokes,
					output_filename
			);

			total_index++;
		}
	}
}

