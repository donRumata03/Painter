#include "painter_pch.h"
#include "signal.h"


#include <tests/cv_io_test.h>
#include <tests/color_test.h>
#include <tests/stroke_rasterization_test.h>
#include <tests/stroke_counting_test.h>
#include <tests/draw_test.h>
#include <tests/stroke_color_optimization_test.h>
#include <tests/error_computing_tests/image_mse_test.h>
#include <tests/GA_tests/stroke_constraining_test.h>
#include <launching/launch_main_ga.h>
#include <tests/GA_tests/population_generation_tests.h>
#include "tests/opencv_test.h"


void posix_death_signal(int signum)
{
	std::cout << console_colors::red << "SegFault :(" << console_colors::remove_all_colors << std::endl;
	signal(signum, SIG_DFL);
	exit(3);
}

int main ()
{
	signal(SIGSEGV, posix_death_signal);

	system(("chcp "s + std::to_string(CP_UTF8)).c_str());

	/// IO tests:

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
	// test_stroke_inverse_counting();
	// test_stroke_derivative_counting();
	test_length_function_by_comparing_with_derivative();

	/// Rasterization:
	// test_stroke_rasterizaton();
	// draw_circle_test();
	// draw_line_test();

	/// Error counting:
	// test_image_mse();
	// test_parallel_image_mse();
	// compare_image_mse_performance_parallel();

	// launch_stroke_constraining_visualizing_test();

	/// Optimization:
	// test_stroke_color_optimization_with_displaying();
	// test_stroke_generation_with_rasterization();

	/// The actual GA:
	// std::string image_path = (fs::path(base_path) / "resources" / "working_tests" / "input" / "brown_coffee.png").string();
	// std::cout << image_path << ":" << std::endl;
	// launch_the_GA(image_path);

	return 0;
}
