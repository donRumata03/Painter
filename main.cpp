#include "painter_pch.h"

#include <tests/cv_io_test.h>
#include <tests/color_test.h>
#include <tests/stroke_rasterization_test.h>
#include <tests/stroke_counting_test.h>
#include <tests/draw_test.h>
#include <tests/stroke_color_optimization_test.h>
#include <tests/error_computing_tests/image_mse_test.h>
#include "tests/opencv_test.h"

int main ()
{
	system(("chcp "s + std::to_string(CP_UTF8)).c_str());

	/// IO tests:

	auto size = cv::Size{ int(1080), int(1920) };

	auto img = cv::Mat(2000, 2000, CV_64FC3, {0., 0., 0., 0. });
	std::cout << img.at<cv::Vec3d>(100, 100) << std::endl;
	std::cout << "Created size" << std::endl;

	show_image_in_system_viewer(img);


	// test_opencv();
	// test_image_processing_cycle();
	// test_image_creation();

	/// Color converting:
	// test_color_converting_to_floating_point();
	// test_color_converting_from_floating_point();
	// custom_color_converting_test();

	/// Simple Utils:
	// test_stroke_counting();
	// color_hash_test();

	/// Rasterization:
	// test_stroke_rasterizaton();
	// draw_circle_test();
	// draw_line_test();

	/// Error counting:
	// test_image_mse();
	// test_parallel_image_mse();
	// compare_image_mse_performance_parallel();

	/// Optimization:
	// test_stroke_color_optimization_with_displaying();

	return 0;
}
