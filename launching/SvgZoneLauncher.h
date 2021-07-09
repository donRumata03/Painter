//
// Created by Vova on 02.03.2021.
//

#pragma once

#include "painter_pch.h"

#include <common_operations/image_segregation.h>
#include <common_operations/image_adaptive_params.h>

#include "optimization/error/error_computing.h"
#include "optimization/stroke_color_optimizing.h"

#include "GA_operations/all.h"

#include "launching/single_zone_workers/GA_worker.h"
#include "launching/single_zone_workers/annealing_worker.h"
#include "multizoner.h"

#include "vector_graphics/SVG_service.h"

#include <common_operations/filesystem_primitives.h>

#include <utility>
#include "utils/Logger.h"

/**
 * Is responsible for dividing the image into several zones
 * following the partitioning made by a pasteurizer such as Adobe Illustrator
 * and expressed in the SVG image given.
 *
 *
 * The ability of running optimizers for multiple zones in parallel is supported.
 *
 * For example, we have 10 worker threads for computing error function
 * and ≈ 10'000 optimization tasks (1 for each zone: to solve the Worker's task for the specified region)
 *
 * It means that it's more than reasonable to distribute the tasks between threads.
 */

template<class OptimizerType>
class SvgZoneLauncher
{
public:
	using OptimizerParameters = typename OptimizerType::ParametersType;


	SvgZoneLauncher(const fs::path& image_path, const CommonStrokingParams& stroking_params, const OptimizerParameters& custom_parameters,
                    const Canvas& canvas, bool parallelize = false, size_t worker_thread_number = std::thread::hardware_concurrency() - 2,
				    fs::path logging_path = fs::path{ painter_base_path } / "log" / "latest");

	void run();
	[[nodiscard]] ComputationalEfficiencyRepresentation get_efficiency_account() const { return efficiency_account; }

    std::vector<colored_stroke> get_final_strokes(Units units = Units::PX, bool shift_strokes = false);
    cv::Size get_image_size() const { return cv::Size(initial_image.cols, initial_image.rows); }

private:

	void worker_function(size_t thread_index);

private:
	Image initial_image;
	std::optional<Canvas> canvas;
	fs::path logging_path;

	CommonStrokingParams stroking_params;
	OptimizerParameters optimizer_parameters;

	size_t zone_number = 0;
	std::vector<size_t> stroke_distribution;
    //std::vector<size_t> iterations_distribution;

	/// Threading:
	bool is_threaded = false;
	size_t thread_worker_number;
	static_thread_pool thread_pool;

	std::mutex common_worker_data_mutex;
	std::vector<std::pair<size_t, size_t>> thread_zone_distribution;

	/// The common data being modified:
	std::optional<SVG_service> svg_manager;
	std::optional<ZoneResourceDistributor> res_distributor;
	// std::vector<OptimizerType> zone_optimizers;

	std::vector<colored_stroke> collected_strokes;

	/// For statistics:
	ComputationalEfficiencyRepresentation efficiency_account;
//	size_t total_computations = 0;
//	size_t total_pixels_processed = 0;
//	double total_time_spent_counting = 0;
};

/// ________________________________________________________________________________________________________________________________________________
/// 															Implementations:
/// ________________________________________________________________________________________________________________________________________________


// TODO: find better place for that function
inline std::string get_current_thread_info(size_t thread_index = 0)
{
    std::stringstream ss;
    ss << "Thread " << thread_index << " (" << std::this_thread::get_id() << ")";
    return ss.str();
}

// TODO: find better place for that function
inline std::string get_range_string(std::pair<size_t, size_t> job_range)
{
    std::stringstream ss;
    if (job_range.first < job_range.second) ss << "[" << job_range.first << ", " << job_range.second - 1 << "]";
    else ss << "ø";
    return ss.str();
}


template <class OptimizerType>
SvgZoneLauncher<OptimizerType>::SvgZoneLauncher (const fs::path& image_path, const CommonStrokingParams& stroking_params,
                                                 const OptimizerParameters& custom_parameters, const Canvas& canvas, bool parallelize,
                                                 size_t worker_thread_number, fs::path logging_path)
			: logging_path(std::move(logging_path)), stroking_params(stroking_params), optimizer_parameters(custom_parameters),
			  canvas(canvas), is_threaded(parallelize), thread_worker_number(worker_thread_number)
{
	// Clear the old log:
	ensure_log_cleared();

	// Determine the number of zones and what the zones actually are
	svg_manager.emplace(image_path, canvas, stroking_params.stroke_width / 2);
	svg_manager->split_paths();

	initial_image = svg_manager->get_raster_original_image();
	save_image(initial_image, (fs::path(painter_base_path) / "log" / "latest" / "original.png").string());

	zone_number = svg_manager->get_shapes_count();

    if (!this->stroking_params.use_absolute_values) {
        this->stroking_params = switch_to_absolute_values(this->stroking_params, initial_image.cols, initial_image.rows);
    }

    res_distributor.emplace(svg_manager.value(), stroking_params.canvas_color);
    stroke_distribution = res_distributor->distribute_resource(stroking_params.stroke_number, 1);

	/// Make task distribution if parallel:

	if (parallelize) {
		thread_zone_distribution = distribute_task_ranges(zone_number, worker_thread_number);
		thread_pool.init(worker_thread_number, [&](size_t thread_index) { this->worker_function(thread_index); }); // TODO: ??
	}
	else {
		thread_zone_distribution = {
				{ 0ULL, zone_number }
		};
	}
}


template <class OptimizerType>
void SvgZoneLauncher<OptimizerType>::worker_function (size_t thread_index)
{
	auto job_range = thread_zone_distribution[thread_index];

	{
		// Inform bout beginning:
		std::lock_guard<std::mutex> print_locker(common_worker_data_mutex);

		LogConsoleInfo("SVG Zone Launcher", get_current_thread_info(thread_index))
            << "Started, job range: " << get_range_string(job_range);
	}

	for (size_t job_index = job_range.first; job_index < job_range.second; ++job_index) {

		std::optional<OptimizerType> optimizer;

		{
			/// Acquire a mutex to work with data, which is common for all the worker threads
			/// this particular data will be used to initialize and launch data
			std::lock_guard<std::mutex> locker(common_worker_data_mutex);

			svg_manager->set_iterator(job_index);

			Image this_zone_image;
			svg_manager->load_current_image(this_zone_image);

			auto this_params = this->stroking_params;

            this_params.stroke_number = stroke_distribution[job_index];
			/*
			this_params.stroke_number = calc_strokes_count(
					this_zone_image, cv::Size { initial_image.rows, initial_image.cols },
					this->stroking_params.stroke_number, stroking_params.canvas_color);*/

			auto this_color = svg_manager->get_current_color();
			this_params.use_constant_color = true;
			this_params.stroke_color = this_color;

            LogInfo("SVG Zone Launcher", get_current_thread_info(thread_index))
                    << "Stroking zone #" << job_index
                    << " (" << this_params.stroke_number << " strokes, " << this_color << " color)";

			optimizer.emplace(this_zone_image, this_params, this->optimizer_parameters, this->logging_path / "stroking" / ("part" + std::to_string(job_index)), false);
		}

		optimizer->run_remaining_iterations();

		{
			/// Acquire mutex to save the data collected:
			std::lock_guard<std::mutex> locker(common_worker_data_mutex);

			efficiency_account = efficiency_account + optimizer->get_efficiency_account();
//			total_computations += optimizer->computations_performed();
//			total_time_spent_counting += optimizer->time_spent_counting();

			svg_manager->set_iterator(job_index);

			auto this_collected_strokes = unpack_stroke_data_buffer(optimizer->get_best_genome());
			colorize_strokes(this_collected_strokes, svg_manager->get_current_color());
			svg_manager->shift_strokes_to_current_box(this_collected_strokes);

			std::copy(this_collected_strokes.begin(), this_collected_strokes.end(), std::back_inserter(collected_strokes));

			LogInfo("SVG Zone Launcher", get_current_thread_info(thread_index)) << "Finish stroking zone #" << job_index;
			Logger::UpdateProgress();
		}
	}

	{
		// Inform about ending:
		std::lock_guard<std::mutex> print_locker(common_worker_data_mutex);
        LogConsoleSuccess("SVG Zone Launcher", get_current_thread_info(thread_index))
                << "Ended, job range: " << get_range_string(job_range);
	}
}




template <class OptimizerType>
void SvgZoneLauncher<OptimizerType>::run ()
{
    LogConsoleInfo("SVG Zone Launcher") << "Run worker(s)";
    Logger::NewProgress(zone_number);
	if(is_threaded) {
	    cv::setNumThreads(0);

		thread_pool.compute();

		cv::setNumThreads(-1); // Reset to system defaults
	} else {
		// Just do all the work with this particular thread:
		worker_function(0);
	}
    LogConsoleSuccess("SVG Zone Launcher") << "End running";
}

template <class OptimizerType>
std::vector<colored_stroke> SvgZoneLauncher<OptimizerType>::get_final_strokes(Units units, bool shift_strokes)
{
    std::vector<colored_stroke> strokes(collected_strokes.size());
    std::copy(collected_strokes.begin(), collected_strokes.end(), strokes.begin());
    if (shift_strokes) svg_manager->shift_strokes_to_canvas(strokes);
    if (units == Units::MM) svg_manager->transform_strokes_into_mm(strokes);
    return strokes;
}

//template <class OptimizerType>
//void SvgZoneLauncher<OptimizerType>::print_diagnostic_information ()
//{
////	std::cout
////			<< "Computations performed: " << total_computations << " (" << optimizer_parameters.computations_expected() << " expected)" << std::endl
////			<< "Average computational time: " << total_time_spent_counting / total_computations * 1e+3 << "ms" << std::endl
////			<< "Computational time per pixel: " << total_time_spent_counting / (total_computations * initial_image) * 1e+9 << "ns" << std::endl
////			<< "=> Computational speed: " << size_t(std::round(1 / average_computation_time_per_pixel_seconds() / 1e+6)) << " MegaPixel / (sec * thread)"
////			<< std::endl;
//}



