//
// Created by Vova on 13.07.2020.
//

#include "color.h"

color get_color_from_hex(const std::string& hex)
{
    std::stringstream ss("0x");
    uint32_t x;

    ss << hex;
    ss >> std::hex >> x;

    return color(((x & 0xFF0000u) >> 16u) / 255.,
                 ((x & 0x00FF00u) >> 8u) / 255.,
                 ((x & 0x0000FFu)) / 255.);
}

void to_json(json& j, const byte_color& col)
{
    j["r"] = col.r;
    j["g"] = col.g;
    j["b"] = col.b;
}

void from_json(const json& j, byte_color& col)
{
    // TODO: assert

    col = byte_color(j.at("r"), j.at("g"), j.at("b"));
}

std::unordered_set<rgb_color<uint8_t>> get_unique_colors (const Image &img)
{
	std::unordered_set<rgb_color<uint8_t>> res;

	img.forEach<cv::Vec3b>([&](cv::Vec3b& pixel, const int position[]){
		byte_color this_color(RGB_Vec(), pixel);
		res.insert(this_color);
	});

	return res;
}

std::vector<rgb_color<uint8_t>> get_color_top_top (const std::unordered_set<rgb_color<uint8_t>> &color_top, size_t amount)
{


	// size_t resultive_color_number = process_color_warning(amount, all_color_number) // std::min(amount, all_color_number);

	// Pick first *resultive_color_number* ones

	/*std::vector<byte_color> res(resultive_color_number);
	for (size_t top_index = 0; top_index < resultive_color_number; ++top_index) {
		res[top_index] = color_top[top_index].first;
	}

	return res;*/
	return {};
}

static size_t process_color_warning(size_t required_amount, size_t existing_amount){
	if (required_amount > existing_amount) {
		std::cout << console_colors::yellow << console_colors::bold <<
		          "WARNING: the number of colors you requested (" << required_amount <<
		          ") is bigger than the total number of unique colors in image (" << existing_amount << ")" <<
		          " => taking the number of unique colors which the image has." <<
		          console_colors::remove_all_colors << std::endl;
	}

	return std::min(required_amount, existing_amount);
}

std::vector<rgb_color<uint8_t>> dummy_get_compressed_unique_colors (const Image &img, size_t amount)
{
	// Make query to get_unique_colors:
	auto all_colors = get_unique_colors(img);

	size_t all_color_number = all_colors.size();

	// Make color rating:
	std::vector<byte_color> all_color_sequence(all_colors.size());
	std::copy(all_colors.begin(), all_colors.end(), all_color_sequence.begin());

	// auto color_top = get_sorted_elements(all_color_sequence);

	return {}; // get_color_top_top(color_top);
}

/**
 * In each iteration it decreases the number of colors by merging some of them in one
 *
 * It`s going with a window through the image
 * The size of the image increases with the index of iteration
 * Iterations are going while the number of unique colors is more than @param amount
 *
 * It can return either a number of colors with higher number than the @param amount or a bit lower
 *
 */
std::unordered_set<byte_color> iterative_get_compressed_unique_colors (const Image &img, size_t amount)
{
	auto all_unique_colors = get_unique_colors(img);
	size_t resultive_target_color_number = process_color_warning(amount, all_unique_colors.size());

	// TODO?

	return std::unordered_set<byte_color>();
}



