//
// Created by Vova on 11.03.2021.
//

#include "contrast_color_finding.h"



template <class ColorDataType>
rgb_color<ColorDataType> find_contrast_color (rgb_color<ColorDataType> initial_color)
{
	using Color = rgb_color<ColorDataType>;

	Color res;
	for (size_t i = 0; i < 3; ++i) {
		res[i] = (initial_color[i] < ColorTraits<Color>::middle ? ColorTraits<Color>::max : ColorTraits<Color>::min);
	}

	return res;
}

// Instantiate for used types:
template color find_contrast_color(color initial_color);
template byte_color find_contrast_color(byte_color initial_color);
