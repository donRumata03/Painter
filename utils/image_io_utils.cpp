//
// Created by Vova on 13.07.2020.
//

#include "image_io_utils.h"


/*
 * Input-output paying respect to floating point and RGB <-> BGR:
 */

image open_image (const std::string &filename)
{
	auto img = cv::imread(filename);

	// Convert BGR image produced by OpenCV to RGB:
	/*
	img.forEach<cv::Vec3d>([](cv::Vec3d& pixel, const int position[]){
		std::swap(pixel[0], pixel[1]);
	});
	*/
	/*auto RGB_image = */ convert_image_between_RGB_and_BGR<byte_Pixel>(img);

	// Convert to image of doubles:
	auto double_img = convert_image_to_floating_point(img);

	return double_img;
}

void cv_show_image (const image &img, const std::string &image_name)
{
	// Convert RGB to BGR to show with OpenCV default image shower:
	/*
	 * img.forEach<cv::Vec3d>([](cv::Vec3d& pixel, const int position[]){
		std::swap(pixel[0], pixel[1]);
	});
	 */

	auto char_image = convert_image_from_floating_point(img);
	convert_image_between_RGB_and_BGR<byte_Pixel>(char_image);

	cv::namedWindow(image_name, cv::WINDOW_GUI_NORMAL);
	cv::imshow(image_name, char_image);

	cv::waitKey(0);
}

void save_image (const image &img, const std::string &filename)
{
	auto char_image = convert_image_from_floating_point(img);
	convert_image_between_RGB_and_BGR<byte_Pixel>(char_image);

	cv::imwrite(filename, char_image);
}

/**
 * Saves image and runs the "system" command:
 */
void show_image_in_system_viewer (const image &img, const std::string &temp_path)
{
	if (temp_path.empty()) {
		// Generate random (chronological) path:
		std::string temp_folder = (fs::path(std::string(base_path)) / "resources"s / "temp"s).string();
		auto prev_temp_files = lsdir(temp_folder);

		// std::string last_file_identifier = "0";
		size_t last_file_identifier = 0;
		for (auto& temp_file : prev_temp_files) {
			auto path_head = split(temp_file, { '/', '\\' }).back();

			// last_file_identifier
		}
	}
}





/*
 * HSV <-> RGB conversions:
 */

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




/*
 * Floating point conversions:
 */

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

std::string get_sample_image_path ()
{
	return (fs::path(std::string(base_path)) / "resources"s / "test_input_images"s / "coffee.jpg"s).string();
}


