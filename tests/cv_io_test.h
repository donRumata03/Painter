//
// Created by Vova on 14.07.2020.
//

#pragma once

#include "utils/image_io_utils.h"

inline void test_image_processing_cycle(){
	std::string image_path = get_sample_image_path();

	auto img = open_image(image_path);
	std::cout << "W: " << img.cols << " H: " << img.rows << std::endl;
	// assert(image.type() == CV_64FC3);
	std::cout << "img[100][200] is " << img.at<cv::Vec3d>(1000, 2000) << std::endl;

	// cv_show_image(img);

	show_image_in_system_viewer(img, "third.png");
}
