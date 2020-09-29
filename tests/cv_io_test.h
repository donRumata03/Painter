//
// Created by Vova on 14.07.2020.
//

#pragma once

#include "utils/image_io_utils.h"

inline void test_image_creation() {
	/*
	auto size = cv::Size{ int(1080), int(1920) };

	auto img = cv::Mat(2000, 2000, CV_64FC3, {0., 0., 0., 0. });
	std::cout << img.at<cv::Vec3d>(100, 100) << std::endl;
	std::cout << "Created image" << std::endl;
	 */

	Image image = make_default_image(1920, 1080);
	std::cout << "Made the image" << std::endl;

	std::cout << image.at<cv::Vec3d>(100, 100) << std::endl;
	image.at<cv::Vec3d>(100, 100) = { 0., 0., 0. };
	std::cout << image.at<cv::Vec3d>(100, 100) << std::endl;

	for (size_t i = 0; i < 1000; ++i) {
		for (size_t j = 0; j < 100; ++j) {
			image.at<cv::Vec3d>(i, j) = { 0., 0., 0. }; // So, indexing is [y, x]
		}
	}

	int max_first = 0, max_second = 0;

	image.forEach<Pixel>([&image, &max_first, &max_second](Pixel& pixel, const int position[]){
		auto first_coord = position[0];
		auto second_coord = position[1];

		max_first = std::max(first_coord, max_first);
		max_second = std::max(second_coord, max_second);

		pixel.z = 0.; // It`s Blue
	});

	std::cout << image.size << std::endl;

	std::cout << "Max x: " << max_first << "Max y: " << max_second << std::endl; // So, indexes are also given [y, x]

	save_image(image, "D:/Projects/Painter/resources/rubbish/test_saving.png");

	// cv_show_image(image);
	show_image_in_system_viewer(image); //, "test_creating_image.png");
}

inline void test_image_processing_cycle(){
	std::string image_path = get_sample_image_path();

	auto img = open_image(image_path);
	std::cout << "W: " << img.cols << " H: " << img.rows << std::endl;
	// assert(image.type() == CV_64FC3);
	std::cout << "img[1000][2000] is " << img.at<cv::Vec3d>(1000, 2000) << std::endl;


	std::mutex mutex;
	img.forEach<Pixel>([&](Pixel& pixel, const int position[]){
		std::lock_guard guard(mutex);
		// std::cout << "img[" << position[0] << "][" << position[1] << "] is " << pixel << std::endl;
	});

	// cv_show_image(img);

	show_image_in_system_viewer(img, "third.png");
}
