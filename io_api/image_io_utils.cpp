//
// Created by Vova on 13.07.2020.
//

#include "image_io_utils.h"

/*
 * Default image constructing:
 */

Image make_default_image (size_t w, size_t h, const color& color)
{
	// std::cout << "Creating" << std::endl;
	auto size = cv::Size{ int(w), int(h) };
	// std::cout << "Created size" << std::endl;

	return cv::Mat(size, CV_64FC3, { color.r, color.g, color.b });
}

Image make_default_image (size_t w, size_t h, double grey_brightness)
{
	return make_default_image(w, h, { grey_brightness, grey_brightness, grey_brightness });
}


/*
 * Input-output paying respect to floating Point and RGB <-> BGR:
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

void save_image (const Image &img, const fs::path &filename)
{
	auto char_image = convert_image_from_floating_point(img);
	convert_image_between_RGB_and_BGR<byte_Pixel>(char_image);

	cv::imwrite(filename.string(), char_image);
}

/**
 * Saves image and runs the "system" command:
 */
void show_image_in_system_viewer (const Image &img, const std::string &temp_name)
{
	std::string temp_folder = (painter_base_path / "resources"s / "temp"s).string();
	// std::cout << "temp folder is: " << temp_folder << std::endl;

	auto space_checker = [](const std::string& which_path, const std::string& path_name){
		if (std::any_of(which_path.begin(), which_path.end(), [](char value){ return value == ' '; })) {
			std::cout << console_colors::red << "Bad " << path_name << ": there are spaces!" << console_colors::remove_all_colors << std::endl;
			assert(false);
		}
	};

	space_checker(temp_folder, "temp path");
	if (!temp_name.empty()) space_checker(temp_name, "user temp file name");

	// std::cout << "Checked spaces" << std::endl;

	std::string filename;
	if (temp_name.empty()) {
		// Generate random (chronological) path:
		auto prev_temp_files = lsdir<given_filename_encoding::utf8>(temp_folder);

		// std::string last_file_identifier = "0";
		size_t last_file_identifier = 0;
		for (auto& temp_file : prev_temp_files) {

			auto path_head = split(temp_file, { '/', '\\' }).back();

			// auto dot_splitted = split(path_head, { '.' });
			// auto naked_name = Slice(dot_splitted, noslice, dot_splitted.size() - 1);

			size_t first_dot_pos = path_head.find('.');
			std::string identifier = Slice(path_head, noslice, first_dot_pos);

			size_t numeric_identifier = 0;

			try {
				numeric_identifier = std::stoll(identifier);
			} catch (std::exception& e) {
				// std::cout << "Exception while converting to long long" << std::endl;
				//
			}

			last_file_identifier = std::max(last_file_identifier, size_t(numeric_identifier));
			// std::cout << temp_file << std::endl;
		}

		// std::cout << "Found last file identifier" << std::endl;

		std::string this_file_identifier = std::to_string(last_file_identifier + 1);
		filename = (fs::path(temp_folder) / (this_file_identifier + ".png")).string();
	}
	else {
		filename = (fs::path(temp_folder) / temp_name).string();
		if (not is_in('.', temp_name)) {
			filename += ".png";
		}
		std::cout << "Showing image in system viewer: Temporary filename is " << filename << std::endl;
	}

	// std::cout << "Found path" << std::endl;

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
 * Floating Point conversions:
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
	return (painter_base_path / "resources"s / "test_input_images"s / "coffee.jpg"s).string();
}


