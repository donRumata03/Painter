//
// Created by Vova on 26.07.2020.
//

#pragma once

#include "rasterization/drawer.h"
#include "test_base.h"

inline void draw_circle_test() {
	size_t image_width = 1920;
	size_t image_height = 1080;

/*
	Image image(image_height, image_width, CV_64FC3, {0., 0., 0.});
*/

	auto image = get_default_image();

	circle c ({image_width / 2., image_height / 2.}, 100);
	draw_circle(image, c, {1., 1., 1.});

	show_image_in_system_viewer(image, "circle_example.png");
}

inline void draw_line_test() {
	size_t image_width = 1920;
	size_t image_height = 1080;

/*
	Image image(image_height, image_width, CV_64FC3, {0., 0., 0.});
*/

	auto image = get_default_image();

	// draw_line(image, {-100, 300}, {1000, 700}, {1., 1., 1.}, 30);
	draw_line_unchecked(image, {-100, 300}, {1000, 700}, {1., 1., 1.}, 30);

	draw_line_unchecked(image, {0, 700}, {1000, 700}, {1., 0.5, 0.5}, 1);


	show_image_in_system_viewer(image, "circle_example.png");
}


