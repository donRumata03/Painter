//
// Created by Vova on 27.09.2020.
//

#pragma once

#include "optimization/error/error_computing.h"
#include "optimization/stroke_color_optimizing.h"

#include "util.h"
#include "data_representation/computational_efficiency_representation.h"


struct RunTimeCounter {
	std::shared_ptr<std::atomic<size_t>> total_runs = std::make_shared<std::atomic<size_t>>(0);
	std::shared_ptr<std::atomic<double>> total_time_seconds = std::make_shared<std::atomic<double>>(0.);
};

class BufferBank
{
public:
	Image get_buffer_for_thread (size_t w, size_t h, const color& canvas_color) {
		size_t thread_id = get_thread_id();

		/// NOT Allocate the buffer if there is one:
		{
			/// Lock while copying all useful data:
			std::lock_guard<std::mutex> lock(buffer_map_realloc_mutex);

			auto proposed_container = buffer_map.find(thread_id);
			if(proposed_container == buffer_map.end()) {
				/// Allocate new buffer:
				auto [iterator, some_bool] = buffer_map.insert({ thread_id, make_default_image(w, h, canvas_color) });
				// std::cout << "Allocated new buffer for thread " << thread_id << std::endl;
				return iterator->second;
			}
			else {
				// Use existing buffer:
				return proposed_container->second;
			}
		}
	}

private:
	std::mutex buffer_map_realloc_mutex;
	mutable std::map<size_t, Image> buffer_map;


	static size_t get_thread_id() {
		size_t res;
		std::stringstream id_receiver;

		id_receiver << std::this_thread::get_id();
		id_receiver >> res;

		return res;
	}


};

class final_fitness_function
{
	ImageStrokingData imageData {};

	size_t total_stroke_number {};
	size_t w{}, h{};

	bool is_run_sequentially {};
	mutable Image personal_buffer {};

	std::shared_ptr<BufferBank> buffer_holder = std::make_shared<BufferBank>();

	bool error_or_fitness {};
	color canvas_color {};

	RunTimeCounter rt_counter;

public:
	final_fitness_function() = default;

	/**
	 * @param is_run_sequentially: if it`s set to true, is stores only one buffer allocated in the constructor
	 * and uses it everywhere. This approach isn`t only applicable for sequential running!!
	 */
	explicit final_fitness_function(
			const ImageStrokingData& imageData,
			size_t strokes,
			bool is_run_sequentially,
			bool _error_or_fitness = false,
			const color& canvas_color = { 0., 0., 0. }
			)
				: imageData(imageData),
				total_stroke_number(strokes),
				is_run_sequentially(is_run_sequentially),
				canvas_color(canvas_color),
				error_or_fitness(_error_or_fitness)
	{
		w = imageData.image.cols;
		h = imageData.image.rows;

		if (is_run_sequentially) {
			personal_buffer = make_default_image(w, h, canvas_color);
		}
	}

	double operator()(const std::vector<double>& stroke_data_buffer) const
	{
		assert(total_stroke_number * 7 == stroke_data_buffer.size()); // Should be dividable by 7 and the result should be equal to 7 * this->total_stroke_number (one double for each stuff)

		Timer computation_timer;

		// Parse the stroke set and compute the colors for them:
		auto strokes = unpack_stroke_data_buffer(stroke_data_buffer);
		for (auto& stroke : strokes) {
		    if (imageData.use_constant_color) find_stroke_color(stroke, imageData.stroke_color);
		    else find_stroke_color(stroke, imageData.image);
		}


		// Compute MSE; OpenCV reference counting system manages the memory properly:

		Image this_buffer = personal_buffer;

		if (!is_run_sequentially) {
			/// Parallel:
			this_buffer = buffer_holder->get_buffer_for_thread(w, h, canvas_color);
		}

		// Rasterize strokes:
		rasterize_strokes(this_buffer, strokes);

		double MSE = image_mse(imageData.image, this_buffer);


		// Clean-up the buffer for this thread:
		auto this_canvas_color = canvas_color;

		auto clean_up_image = [this_canvas_color](Image& image_to_clean_up){
			image_to_clean_up.forEach<Pixel>([this_canvas_color] (Pixel &pixel, const int position[]) {
				pixel.x = this_canvas_color.r;
				pixel.y = this_canvas_color.g;
				pixel.z = this_canvas_color.b;
			});
		};

		clean_up_image(this_buffer);

		rt_counter.total_time_seconds->operator+=(computation_timer.get_time(Timer::time_units::seconds));
		rt_counter.total_runs->operator++();

		return error_or_fitness ? MSE : (1 / MSE);
	}

	/// Statistics:

	auto get_efficiency_account() const {
		return ComputationalEfficiencyRepresentation{
			.computations_performed = *rt_counter.total_runs,
			.pixels_processed = *rt_counter.total_runs * w * h,
			.time_spent_computing = (*rt_counter.total_time_seconds),
		};
	}

//	double computations_performed() const {
//		return *rt_counter.total_runs;
//	}
//
//	double average_computation_time_seconds() const {
//		return (*rt_counter.total_time_seconds) / (*rt_counter.total_runs);
//	}
//
//	double average_computation_time_per_pixel_seconds() const {
//		double area = double(w) * double(h);
//
//		return average_computation_time_seconds() / area;
//	}
//
//	double time_spent_counting() const
//	{
//		return (*rt_counter.total_time_seconds);
//	}
};


