//
// Created by Vova on 13.07.2020.
//

#pragma once

#include "color.h"


void cv_show_image (const image &img, const std::string &image_name = "Image"); // Assume image is in RGB, elements are of type double
image open_image(const std::string& filename);                     // Resultant image is in RGB, type is double
void save_image(const image &img, const std::string &filename);  // Assume image is in RGB, elements are of type double
void show_image_in_system_viewer (const image &img, const std::string &temp_path = ""); // Assume image is in RGB, elements are of type double


template <class Pixel_type> void convert_image_between_RGB_and_BGR(image& img);
template <class Pixel_type> image convert_image_between_RGB_and_BGR(const image& img);

image convert_from_rgb_to_hsv(const image& img);
image convert_from_hsv_to_rgb(const image& img);


image convert_image_to_floating_point(const image& img); // From uint8_t 3 channels to double
image convert_image_from_floating_point(const image& img); // From double 3 channels to uint8_t


std::string get_sample_image_path();




// Inline template implementations:


/*
 * RGB swapping conversions:
 */

template <class Pixel_type>
void convert_image_between_RGB_and_BGR (image &img)
{
	img.forEach<Pixel_type>([](Pixel_type& pixel, const int position[]){
		std::swap(pixel.x, pixel.z);
	});
}

template <class Pixel_type>
image convert_image_between_RGB_and_BGR (const image &img)
{
	auto new_image = img.clone();
	convert_image_between_RGB_and_BGR(new_image);
	return new_image;
}

