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
#include <tests/example_stroke_image_generation.h>
#include <tests/image_splitting_test.h>
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

    // system(("chcp "s + std::to_string(CP_UTF8)).c_str());

    set_utf8_in_console();
	std::cout << "Using " << std::thread::hardware_concurrency() - 1 << " threads of " << std::thread::hardware_concurrency() << std::endl;

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
	// test_length_function_by_comparing_with_derivative();
	// test_stroke_scaling();

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

	/// Splitting:
	test_zone_scheduling();

	/// GA util tests:
	// test_stroke_generation_with_rasterization();
	// test_stroke_generation();
	// launch_stroke_constraining_visualizing_test();
	// generate_stoke_image_example();

	/// The actual GA:
	std::string coffee_path = (fs::path(painter_base_path) / "images_for_testing" / "brown_coffee.png").string();
	std::string van_gogh_path = (fs::path(painter_base_path) / "images_for_testing" / "van-gogh.jpg").string();

	std::string one_stroke_path = (fs::path(painter_base_path) / "images_for_testing" / "one_stroke.png").string();

	std::string circle_path = (fs::path(painter_base_path) / "images_for_testing" / "circle.png").string();
	std::string moon_path = (fs::path(painter_base_path) / "images_for_testing" / "moon.png").string();
	std::string cutted_moon_path = (fs::path(painter_base_path) / "images_for_testing" / "cutted_moon.png").string();
	std::string half_ring_path = (fs::path(painter_base_path) / "images_for_testing" / "half_ring.png").string();

	// const auto& image_path = coffee_path;
	// const auto& image_path = van_gogh_path;

	const auto& image_path = one_stroke_path;

	// const auto& image_path = circle_path;
	// const auto& image_path = moon_path;
	// const auto& image_path = cutted_moon_path;
	// const auto& image_path = half_ring_path;

	// launch_the_GA(image_path);



	return 0;
}
