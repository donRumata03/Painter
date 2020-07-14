//
// Created by Vova on 13.07.2020.
//

#include "image_io_utils.h"


image open_image (const std::string &filename)
{
	auto img = cv::imread(filename);

	// Convert BGR image produced by OpenCV to RGB:
	/*
	img.forEach<cv::Vec3d>([](cv::Vec3d& pixel, const int position[]){
		std::swap(pixel[0], pixel[1]);
	});
	*/
	/*auto RGB_image = */
	convert_image_between_RGB_and_BGR(img);

	// Convert to image of doubles:
	auto double_img = convert_image_to_floating_point(img);

	return double_img;
}

void show_image (const image &img, const std::string &image_name)
{
	// Convert RGB to BGR to show with OpenCV default image shower:

	/*
	 * img.forEach<cv::Vec3d>([](cv::Vec3d& pixel, const int position[]){
		std::swap(pixel[0], pixel[1]);
	});
	 */

	auto BGR_image = convert_image_between_RGB_and_BGR(img);

	cv::namedWindow(image_name, cv::WINDOW_GUI_NORMAL);
	cv::imshow(image_name, BGR_image);

	cv::waitKey(0);
}


image convert_from_rgb_to_hsv (const image &img)
{
	image res;
	cv::cvtColor(img, res, cv::COLOR_RGB2HSV);

	return res;
}


image convert_from_hsv_to_rgb (const image &img)
{
	image res;
	cv::cvtColor(img, res, cv::COLOR_HSV2RGB);

	return res;
}




void convert_image_between_RGB_and_BGR (image &img)
{
	img.forEach<Pixel>([](Pixel& pixel, const int position[]){
		std::swap(pixel.x, pixel.z);
	});
}

image convert_image_between_RGB_and_BGR (const image &img)
{
	auto new_image = img.clone();
	convert_image_between_RGB_and_BGR(new_image);
	return new_image;
}



image convert_image_to_floating_point (const image &img)
{
	image res;
	img.convertTo(res, CV_64FC3);
	return res;
}

image convert_image_from_floating_point (const image &img)
{
	image res;
	img.convertTo(res, CV_8UC3);
	return res;
}



