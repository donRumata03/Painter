//
// Created by Vova on 14.07.2020.
//

#pragma once

#include "painter_pch.h"
#include "utils/color.h"

// Define a pixel
// typedef cv::Point3_<uint8_t> Pixel;

// A complicated threshold is defined so
// a non-trivial amount of computation
// is done at each pixel.
void complicatedThreshold(byte_Pixel &pixel)
{
	if (pow(double(pixel.x)/10,2.5) > 100)
	{
		pixel.x = 255;
		pixel.y = 255;
		pixel.z = 255;
	}
	else
	{
		pixel.x = 0;
		pixel.y = 0;
		pixel.z = 0;
	}
}

inline void test_opencv() {

	cv::Mat image = cv::imread(R"(D:\Projects\Tests\Opencv_test\bg.jpg)"); // cv::Mat::zeros(100, 100, CV_8U);

	std::cout << std::boolalpha << image.type() << " " << CV_8UC3 << " " << (image.type() == CV_8UC3) << std::endl;

	image.forEach<byte_Pixel>([&](byte_Pixel& pixel, const int position[]){
		complicatedThreshold(pixel);
	});


	for (auto x = size_t(image.cols / 3); x < size_t(image.cols * 2 / 3); x++) {
		for (auto y = size_t(image.rows / 3); y < size_t(image.rows * 2 / 3); y++) {
			// std::cout << x << " " << y << std::endl;
			auto& vec = image.at<cv::Vec3b>(y, x);
			// std::cout << int(vec[0]) << " " << int(std::clamp(vec[0] * 2, 0, 255)) << " " << int(vec[1]) << " " << int(vec[2]) << " " << vec << std::endl;
			vec[0] = std::clamp(std::intmax_t(vec[0]) * 2 + 20, std::intmax_t(0), std::intmax_t(255));
			if (!(x % 10) || !(y % 10)) vec = { 34, 54, 232 };
		}
	}

	// typedef cv::Point3_<uint8_t> Pixel;
	// image.forEach<Pixel>([&](Pixel& pixel, const int position[]) -> void {
// std::cout « position[0] « "\n\n\n\n" « position[1] « "\n\n\n\n" « position[2] « "\n\n\n\n";
// std::cout « int(pixel.x) « "\n" « int(pixel.y) « "\n" « int(pixel.z) « std::endl;
// pixel.x = ;
// pixel.y = 255;
// pixel.z = 255;
	// });

	std::cout << image.rows << std::endl;
	std::cout << image.cols << std::endl;


//	unsigned char& c = image.at<uint8_t>(1000, 100);
//
//	std::cout << int(c) << std::endl;
//
////for(int i = 0; i < image.rows; i++)
//// for(int j = 0; j < image.cols; j++)
//// image.at<double>(i,j)=1./(i+j+1);

	convert_image_between_RGB_and_BGR(image);
	cv::imwrite("D:/Projects/Painter/resources/test_saving.jpg", image);

	cv::namedWindow("Test window", cv::WINDOW_GUI_NORMAL);
	cv::imshow("Test window", image);

	cv::waitKey(0);

	std::cout << "Hello, world!" << std::endl;

	std::cout << size_t(image.cols / 3) << " " << size_t(image.cols * 2 / 3) << std::endl;


	// std::string s = std::getenv("OPENCV_DIR");
	// std::cout << s << std::endl;
}
