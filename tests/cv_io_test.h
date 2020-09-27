//
// Created by Vova on 14.07.2020.
//

#pragma once

#include "utils/image_io_utils.h"

inline void test_image_creation() {
	Image image = make_default_image(1920, 1080);
	show_image_in_system_viewer(image);
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
