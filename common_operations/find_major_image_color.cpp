//
// Created by Vova on 11.03.2021.
//

#include "find_major_image_color.h"


/// As for now assume that that major color is the brightest
// TODO: think something more clever out?
color find_major_image_color (const Image& image)
{
	color res = { 0., 0., 0. };
	double current_brightness = 0;

	image.forEach<Pixel>([&res, &current_brightness](Pixel& pixel, const int position[]){
		auto this_col = color{pixel};
		auto this_brightness = this_col.brightness();
		if (this_brightness > current_brightness) {
			res = this_col;
			current_brightness = this_brightness;
		}
	});

	return res;
}
