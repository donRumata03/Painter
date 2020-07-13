#include <iostream>

#include "painter_pch.h"

#include "utils/image_io_utils.h"

int main ()
{
	
	cv::Mat image = cv::imread(R"(D:\Projects\Tests\Opencv_test\bg.jpg)"); // cv::Mat::zeros(100, 100, CV_8U);

	std::cout << std::boolalpha << image.type() << " " << CV_8UC3 << " " << (image.type() == CV_8UC3) << std::endl;

	for (auto x = size_t(image.cols / 3); x < size_t(image.cols * 2 / 3); x++) {
		for (auto y = size_t(image.rows / 3); y < size_t(image.rows * 2 / 3); y++) {
			// std::cout << x << " " << y << std::endl;
			auto& vec = image.at<cv::Vec3b>(y, x);
			// std::cout << int(vec[0]) << " " << int(std::clamp(vec[0] * 2, 0, 255)) << " " << int(vec[1]) << " " << int(vec[2]) << " " << vec << std::endl;
			vec[0] = std::clamp(vec[0] * 2, 0, 255);
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

	cv::namedWindow("Test window", cv::WINDOW_GUI_NORMAL);
	cv::imshow("Test window", image);

	cv::waitKey(0);

	std::cout << "Hello, world!" << std::endl;

	std::cout << size_t(image.cols / 3) << " " << size_t(image.cols * 2 / 3) << std::endl;

	// std::string s = std::getenv("OPENCV_DIR");
	// std::cout << s << std::endl;
	return 0;
}