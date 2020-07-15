//
// Created by Vova on 13.07.2020.
//

#include "image_io_utils.h"


/*
 * Input-output paying respect to floating point and RGB <-> BGR:
 */

Image open_image (const std::string &filename)
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

void cv_show_image (const Image &img, const std::string &image_name)
{
	// Convert RGB to BGR to show with OpenCV default image shower:
	/*
	 * img.forEach<cv::Vec3d>([](cv::Vec3d& pixel, const int position[]){
		std::swap(pixel[0], pixel[1]);
	});
	 */

	auto char_image = convert_image_from_floating_point(img);
	convert_image_between_RGB_and_BGR<byte_Pixel>(char_image);

	cv::namedWindow(image_name, cv::WINDOW_NORMAL);
	cv::imshow(image_name, char_image);

	cv::waitKey(0);
}

void save_image (const Image &img, const std::string &filename)
{
	auto char_image = convert_image_from_floating_point(img);
	convert_image_between_RGB_and_BGR<byte_Pixel>(char_image);

	cv::imwrite(filename, char_image);
}

/**
 * Saves image and runs the "system" command:
 */
void show_image_in_system_viewer (const Image &img, const std::string &temp_name)
{
	std::string temp_folder = (fs::path(std::string(base_path)) / "resources"s / "temp"s).string();
	// std::cout << "temp folder is: " << temp_folder << std::endl;

	std::string filename;
	if (temp_name.empty()) {
		// Generate random (chronological) path:
		auto prev_temp_files = lsdir(temp_folder);

		// std::string last_file_identifier = "0";
		size_t last_file_identifier = 0;
		for (auto& temp_file : prev_temp_files) {
			auto path_head = split(temp_file, { '/', '\\' }).back();

			// auto dot_splitted = split(path_head, { '.' });
			// auto naked_name = Slice(dot_splitted, noslice, dot_splitted.size() - 1);

			size_t first_dot_pos = path_head.find('.');
			std::string identifier = Slice(path_head, noslice, first_dot_pos);
			last_file_identifier = std::max(last_file_identifier, size_t(std::stoll(identifier)));
		}

		std::string this_file_identifier = std::to_string(last_file_identifier + 1);
		filename = (fs::path(temp_folder) / (this_file_identifier + ".png")).string();
	}
	else {
		filename = (fs::path(temp_folder) / temp_name).string();
		std::cout << "Showing image in system viewer: Temporary filename is " << filename << std::endl;
	}

	save_image(img, filename);

	system(filename.c_str());
}





/*
 * HSV <-> RGB conversions:
 */

Image convert_from_rgb_to_hsv (const Image &img)
{
	Image res;
	cv::cvtColor(img, res, cv::COLOR_RGB2HSV);

	return res;
}


Image convert_from_hsv_to_rgb (const Image &img)
{
	Image res;
	cv::cvtColor(img, res, cv::COLOR_HSV2RGB);

	return res;
}




/*
 * Floating point conversions:
 */

Image convert_image_to_floating_point (const Image &img)
{
	Image res;
	img.convertTo(res, CV_64FC3, 1./255);
	return res;
}

Image convert_image_from_floating_point (const Image &img)
{
	Image res;
	img.convertTo(res, CV_8UC3, 255.);
	return res;
}

std::string get_sample_image_path ()
{
	return (fs::path(std::string(base_path)) / "resources"s / "test_input_images"s / "coffee.jpg"s).string();
}


