//
// Created by Vova on 29.09.2020.
//

#pragma once

#include <optimization/stroke_color_optimizing.h>
#include <rasterization/stroke_rasterizer.h>

#include <utility>
#include "data_representation/stroke.h"


struct ImageStrokingData {
public:
    ImageStrokingData() = default;
    ImageStrokingData(Image image, bool use_constant_color = false, const color& stroke_color = { 1., 1., 1.})
            : image(std::move(image)), use_constant_color(use_constant_color), stroke_color(stroke_color) {}

    Image image;

    bool use_constant_color = false;
    color stroke_color {};

};


/**
 * The returning buffer actually doesn`t have colors.
 * The strokes are marked as colored for performance reasons.
 */
// template<bool color_stored = false>
inline std::vector<colored_stroke> unpack_stroke_data_buffer(const std::vector<double>& stroke_data_buffer) {
	assert(sizeof(stroke) == 7 * sizeof(double)); // This format is assumed
	assert(!(stroke_data_buffer.size() % 7)); // Should be dividable by 7
	auto total_stroke_number = stroke_data_buffer.size() / 7;

	std::vector<colored_stroke> strokes;
	strokes.resize(total_stroke_number);

	for (size_t stroke_index = 0; stroke_index < total_stroke_number; ++stroke_index) {
		size_t stroke_value_initial_index = 7 * stroke_index;

		/*auto &unpacked_colored_stroke = */
		reinterpret_cast<stroke&>(strokes[stroke_index]) = stroke {
						{ stroke_data_buffer[stroke_value_initial_index],
						  stroke_data_buffer[stroke_value_initial_index + 1] },
						{ stroke_data_buffer[stroke_value_initial_index + 2],
						  stroke_data_buffer[stroke_value_initial_index + 3] },
						{ stroke_data_buffer[stroke_value_initial_index + 4],
						  stroke_data_buffer[stroke_value_initial_index + 5] },
						stroke_data_buffer[stroke_value_initial_index + 6]
				};

		// strokes[stroke_index].width = double{}; // <- The stroke color is undefined at this moment
	}

	return strokes;
}

inline void colorize_strokes (std::vector<colored_stroke>& strokes_to_colorize, const Image& image)
{
	for (auto& stroke : strokes_to_colorize) find_stroke_color(stroke, image);
};

inline void colorize_strokes (std::vector<colored_stroke>& strokes_to_colorize, const color& setup_color)
{
    for (auto& stroke : strokes_to_colorize) find_stroke_color(stroke, setup_color);
};

inline void colorize_strokes (std::vector<colored_stroke>& strokes_to_colorize, const ImageStrokingData& data)
{
    if (data.use_constant_color) colorize_strokes(strokes_to_colorize, data.stroke_color);
    else colorize_strokes(strokes_to_colorize, data.image);
};


template<
        class ColoredOrNotStroke,
        std::enable_if_t<std::is_same_v<ColoredOrNotStroke, stroke> or std::is_same_v<ColoredOrNotStroke, colored_stroke>, int*> nothing = nullptr
>
// template<bool store_color>
inline std::vector<double> pack_stroke_data(const std::vector<ColoredOrNotStroke>& strokes) {
	// constexpr size_t stroke_size_in_doubles = store_color ? 7;

	// std::cout << "Started packing" << std::endl;

	auto stroke_number = strokes.size();
	std::vector<double> stroke_data_buffer(stroke_number * 7);

	for (size_t stroke_index = 0; stroke_index < stroke_number; ++stroke_index) {
		const auto& the_stroke = strokes[stroke_index];
		size_t base_index = stroke_index * 7;

		stroke_data_buffer[base_index]     = the_stroke.p0.x;
		stroke_data_buffer[base_index + 1] = the_stroke.p0.y;

		stroke_data_buffer[base_index + 2] = the_stroke.p1.x;
		stroke_data_buffer[base_index + 3] = the_stroke.p1.y;

		stroke_data_buffer[base_index + 4] = the_stroke.p2.x;
		stroke_data_buffer[base_index + 5] = the_stroke.p2.y;

		stroke_data_buffer[base_index + 6] = the_stroke.width;
	}

	// std::cout << "Ended packing" << std::endl;

	return stroke_data_buffer;
}

inline Image stroke_buffer_to_image(const std::vector<double>& stroke_buffer, const ImageStrokingData& data, color canvas_color = { 0., 0., 0., }) {
	auto strokes = unpack_stroke_data_buffer(stroke_buffer);
	colorize_strokes(strokes, data);

	Image new_image = make_default_image(data.image.cols, data.image.rows, canvas_color);
	rasterize_strokes(new_image, strokes);

	return new_image;
}
/*
inline Image stroke_buffer_to_image(const std::vector<double>& stroke_buffer, const Image& original_image) {
    auto strokes = unpack_stroke_data_buffer(stroke_buffer);
    colorize_strokes(strokes, original_image);

    Image new_image = make_default_image(original_image.cols, original_image.rows);
    rasterize_strokes(new_image, strokes);

    return new_image;
}

inline void save_stroke_buffer_as_image(const std::vector<double>& stroke_buffer, const Image& original_image, const fs::path& filename) {
	Image image = stroke_buffer_to_image(stroke_buffer, original_image);
	save_image(image, filename);
}

inline void save_stroke_buffer_as_image(const std::vector<double>& stroke_buffer, const color& stroke_color, const fs::path& filename) {
    Image image = stroke_buffer_to_image(stroke_buffer, stroke_color);
    save_image(image, filename);
}*/

inline void save_stroke_buffer_as_image(const std::vector<double>& stroke_buffer, const ImageStrokingData& data, const fs::path& filename, color canvas_color) {
    Image image = stroke_buffer_to_image(stroke_buffer, data, canvas_color);
    save_image(image, filename);
}


inline void save_stroke_buffer_as_images(const std::vector<double>& stroke_buffer, const ImageStrokingData& data, const std::vector<fs::path>& filenames, color canvas_color) {
	Image image = stroke_buffer_to_image(stroke_buffer, data, canvas_color);
	for (auto& filename : filenames)
		save_image(image, filename);
}

//////////////////////								Non-unique functionality:		////////////////////////////////////////

inline double transfer_range (double value, std::pair<double, double> from_range, std::pair<double, double> to_range)
{
	double size_first = from_range.second - from_range.first;
	double size_second = to_range.second - to_range.first;

	return to_range.first + (size_second * (value - from_range.first) / size_first);
}

