#include "painter_pch.h"
#include "signal.h"


#include <tests/cv_io_test.h>
#include <tests/color_test.h>
#include <tests/rasterization_tests/stroke_rasterization_algorithm_demonstration.h>
#include <tests/stroke_counting_test.h>
#include <tests/draw_test.h>
#include <tests/stroke_color_optimization_test.h>
#include <tests/error_computing_tests/image_mse_test.h>
#include <tests/constraining_tests/stroke_constraining_test.h>
#include <launching/launch_stroking.h>
#include <tests/GA_tests/population_generation_tests.h>
#include <tests/example_stroke_image_generation.h>
#include <tests/image_splitting_test.h>
#include <tests/strokes_to_json_tests.h>
#include <tests/svg_service_test.h>
#include <tests/area_counting_test.h>
#include <tests/param_adapting_test.h>
#include <tests/test_stroke_sorting.h>
#include "tests/opencv_test.h"
#include <tests/test_logging.h>
#include <tests/constraining_tests/problem_constraining_test.h>
#include <tests/rasterization_tests/opencv_alpha_chanel_test.h>
#include <tests/cv_tests/threading_test.h>
#include "utils/Logger.h"


void posix_death_signal(int signum)
{
	std::cout << console_colors::red << "SegFault :(" << console_colors::remove_all_colors << std::endl;
	signal(signum, SIG_DFL);
	exit(3);
}

int main ()
{
	signal(SIGSEGV, posix_death_signal);

    set_utf8_in_console();
    Logger::SetLogFile(painter_base_path / "log" / "logs.txt");
    Logger::SetLoggingLevel(LogLevel::Info);

    Console() << console_colors::green << "Welcome to Painter™!" << console_colors::remove_all_colors;
    LogConsoleInfo("Main") << "Using up to " << std::thread::hardware_concurrency() - 1 << " threads of " << std::thread::hardware_concurrency();

    /// Example Paths:
	std::string coffee_path = (fs::path(painter_base_path) / "images_for_testing" / "brown_coffee.png").string();
	std::string van_gogh_path = (fs::path(painter_base_path) / "images_for_testing" / "van-gogh.jpg").string();
	std::string van_gogh_beard_path = (fs::path(painter_base_path) / "images_for_testing" / "van-gogh-beard.png").string();

	std::string one_stroke_path = (fs::path(painter_base_path) / "images_for_testing" / "one_stroke.png").string();

	std::string circle_path = (fs::path(painter_base_path) / "images_for_testing" / "circle.png").string();
	std::string cutted_circle_path = (fs::path(painter_base_path) / "images_for_testing" / "circle_cutted.png").string();
	std::string moon_path = (fs::path(painter_base_path) / "images_for_testing" / "moon.png").string();
	std::string cutted_moon_path = (fs::path(painter_base_path) / "images_for_testing" / "cutted_moon.png").string();
	std::string half_ring_path = (fs::path(painter_base_path) / "images_for_testing" / "half_ring.png").string();

	std::string simple_svg_path = (fs::path(painter_base_path) / "images_for_testing" / "simple.svg").string();
	std::string van_gogh_svg_path = (fs::path(painter_base_path) / "images_for_testing" / "van-gogh.svg").string();

	std::string zone_example_path = (fs::path(painter_base_path) / "images_for_testing" / "zone_example.png").string();
	std::string strange_zone_path = (fs::path(painter_base_path) / "images_for_testing" / "strange_zone.png").string();


	const auto& image_path = simple_svg_path;
	LogConsoleInfo("Main") << "Example Image Path: " << image_path;
	Console() << "_________________________________________________________________________";

	Canvas canvas(300, 400, 96);


	/// IO tests:

	// test_opencv();
	// test_image_processing_cycle();
	// test_image_creation();
	// test_cv_threading();

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
	// test_logging();

	// test_painted_area_counting();
	// test_painted_figures_perimeters_counting();

	/// Rasterization:
	// compare_stroke_rasterizaton_algos();
	// test_intersection_alpha_channel_in_opencv();
	// draw_circle_test();
	// draw_line_test();

	/// Error counting:
	// test_image_mse();
	// test_parallel_image_mse();
	// compare_image_mse_performance_parallel();


	/// Optimization:
	// test_stroke_color_optimization_with_displaying();

	/// Splitting image into rectangular zones:
	// test_zone_scheduling();
	// test_image_segregation();

	/// Json
	// test_stroke_to_json();

	/// GA util tests:
	// test_stroke_generation_with_rasterization();
	// test_stroke_generation();
	// launch_stroke_constraining_visualizing_test();
	// problematic_stroke_constraining_test(image_path);
	// generate_stoke_image_example();

	/// Working with Zones:
	// test_zone_distributor(image_path);
	// test_counting_zone_inequality(image_path);
	// test_plotting_inequality_graph(image_path);

	/// Sorting strokes:
	// test_zone_number_determining();
	// test_stroke_sorting_function(image_path);

	/// SVG management:
	// test_splitting_SVG(van_gogh_svg_path);

	/// Single zone:
	// launch_single_zone_GA(image_path);
    // launch_single_zone_annealing(image_path, true);

	/// Multizone:
	// launch_multizone_GA(image_path);

	/// SVG launch:
    launch_svg_stroking(image_path);
	// launch_svg_zone_stroking<GA_worker>(image_path, canvas);
    // launch_svg_zone_stroking<AnnealingWorker>(image_path, canvas);



    Logger::Stop();
	return 0;
}
