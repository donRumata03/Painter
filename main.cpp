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

	// test_opencv();
	// test_image_processing_cycle();


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
	test_image_mse();

	/// Optimization:
	test_stroke_color_optimization_with_displaying();

	return 0;
}
