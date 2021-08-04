//
// Created by vova on 04.08.2021.
//


#pragma once

#include "rasterization/PixelLayerTracker.h"



inline void test_pixel_layer_tracking() {
	color red = { 1., 0., 0. };
	color green = { 0., 1., 0. };
	color blue = { 0., 0., 1. };


	PixelLayerTracker tracker;

	auto col_0 = tracker.get_pixel_color();

	tracker.add_color_layer(10, red);
	tracker.add_color_layer(20, green);
	tracker.add_color_layer(1, blue);

	auto c2 = tracker.get_pixel_color();

	tracker.remove_layer_by_priority(20);
	auto c3 = tracker.get_pixel_color();

	tracker.remove_layer_by_priority(1);
	auto c4 = tracker.get_pixel_color();

	tracker.remove_layer_by_priority(10);
	auto c5 = tracker.get_pixel_color();
}

