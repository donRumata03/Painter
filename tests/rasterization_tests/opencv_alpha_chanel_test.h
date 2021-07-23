//
// Created by Vova on 23.07.2021.
//

#pragma once

#include <tests/_test_common_utils.h>
#include <io_api/image_io_utils.h>


inline void test_intersection_alpha_channel_in_opencv() {
	Image image(make_ocv_size(1920, 1080), CV_8UC4, { 0, 0, 0, 255 });

	auto image_center = get_image_range_limits(image).get_center();
	circle circle(
			{ image_center.x * 0.4, image_center.y },
			0.3 * (1080. / 2)
	);

	while (circle.center.x < image_center.x * (2 - 0.4)) {
		cv::circle(
				image,
				cv::Point{ int(std::round(circle.center.x)), int(std::round(circle.center.y)) }, int(std::round(circle.r)),
				{ 255, 255, 255, 13 }, cv::FILLED
		);

		circle.center.x += image_center.x * 0.05;
	}
	// rasterize_stroke(image, strk);

	std::string filename = (painter_base_path / "tests" / "rasterization_tests" / "alpha_circle.png").string();
	cv::imwrite(filename, image);
	system(filename.c_str());

	// show_image_in_system_viewer(image);
	// cv_show_image(image);


//	cv::namedWindow("AlphaCircle", cv::WINDOW_NORMAL);
//	cv::imshow("AlphaCircle", image);
//
//	cv::waitKey(0);
}
