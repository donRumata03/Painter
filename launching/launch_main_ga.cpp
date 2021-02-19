//
// Created by Vova on 02.10.2020.
//

#include "launch_main_ga.h"

#include "io_api/image_io_utils.h"
#include "GA_worker.h"
#include "GA_parameter_sets.h"

void launch_single_zone_GA (const std::string &filename)
{
	Image image = open_image(filename);

	GA_launching_params this_params = default_params;

	GA_worker worker(image, this_params);
	worker.run_remaining_iterations();

	/// For one thread:
	// Average computational time: 1.96496ms
	// Computational time per pixel: 13.0997 (ns/pix)
	// => Speed is 77 MegaPix / (sec * thread)

	/// For many threads without memory allocation saving system:
	// Average computational time: 8.70384ms
	// Computational time per pixel: 58.0256ns
	// => Computational speed: 17 MegaPixel / (sec * thread)

	/// For many threads with memory allocation saving system (measured WITH buffer getting):
	// 31 MegaPixel / (sec * thread)

	/// For many threads with memory allocation saving system (measured WITHOUT buffer getting):
	// 31 MegaPixel / (sec * thread)

	worker.print_diagnostic_information();
	worker.show_fitness_dynamic();
}

void launch_multizone_GA (const std::string& filename)
{
	GA_launching_params this_params = default_params;
	image_splitting_params this_splitting_params = van_gogh_splitting_params;

	Image image = open_image(filename);
	multizone_GA_launcher launcher(image,
								this_splitting_params.zones_x, this_splitting_params.zones_y, this_splitting_params.overlay_percent,
								this_params);
	std::cout << "[main launching function]: Performed initialization. Running.." << std::endl;

	launcher.run();

	launcher.save_result(painter_base_path / "log" / "latest" / "result.png");
}

void launch_svg_stroking(const std::string &filename) {
    fs::remove_all(painter_base_path / "log" / "latest");

    SVG_service service(filename);
    service.split_paths();

    Image image;
    std::vector<colored_stroke> strokes;
    while (service.load_current_image(image))
    {
        GA_launching_params params = default_params;

        GA_worker worker(image, params,
                         fs::path(painter_base_path) / "log" / "latest" / ("part" + std::to_string(service.get_it())));
        worker.run_remaining_iterations();

        worker.print_diagnostic_information();
        // worker.show_fitness_dynamic();

        auto cur_strokes = unpack_stroke_data_buffer(worker.get_best_genome());
        colorize_strokes(cur_strokes, image); // TODO: Use specific color of image
        service.shift_strokes(cur_strokes);
        strokes.insert(strokes.end(), cur_strokes.begin(), cur_strokes.end());

        service.next();
    }

    // View result
    image = make_default_image(service.get_borders().width, service.get_borders().height);
    rasterize_strokes(image, strokes);
    save_image(image, (fs::path(painter_base_path) / "log" / "latest" / "result.png").string());

    // Save strokes
    std::vector<color> pallete; // TODO: use specific color for strokes
    for (auto& col_stroke : strokes) {
        if (std::find(pallete.begin(), pallete.end(), col_stroke.background_color) == std::end(pallete)) {
            pallete.emplace_back(col_stroke.background_color);
        }
    }
    json j;
    to_json(j, strokes, pallete);
    std::ofstream json_file(fs::path(painter_base_path) / "log" / "latest" / "result.json");
    json_file << j;
    json_file.close();

    // Debug show
    Image result = image.clone();
    convert_image_between_RGB_and_BGR<Pixel>(result);
    cv::imshow("result", result);
    cv::waitKey(0);
}


multizone_GA_launcher::multizone_GA_launcher (Image _image, size_t _zones_x, size_t _zones_y,
                                              double overlay_percent, GA_launching_params params)
							: zones(
									split_image_into_zones(
											_image,
											schedule_image_splitting(_image.cols, _image.rows, _zones_x, _zones_y, overlay_percent)
											)
										), zones_x(_zones_x), zones_y(_zones_y)
{
	image_w = this->image.cols;
	image_h = this->image.rows;

	image = std::move(_image);

	/// Init logger:
	logger = image_logging_callback(
			image,
			(painter_base_path / "log" / "latest").string(), 0, false);


	/// Init workers:
	std::vector<bool> worker_ready_mask(zones_y, false);
	workers_ready.assign(zones_x, worker_ready_mask);

	workers.reserve(zones_x);

	for (size_t worker_x_index = 0; worker_x_index < zones_x; ++worker_x_index) {
		auto& worker_col = workers.emplace_back();
		worker_col.reserve(zones_y);

		for (size_t worker_y_index = 0; worker_y_index < zones_y; ++worker_y_index) {
			std::string this_path = "x_index=" + std::to_string(worker_x_index) + ",y_index=" + std::to_string(worker_y_index);

			std::shared_ptr<GA_worker> this_worker = std::make_shared<GA_worker>(
					zones.images[worker_x_index][worker_y_index], params, painter_base_path / "log" / "latest" / this_path
					);

			worker_col.emplace_back(std::move(this_worker));
		}
	}


}


bool multizone_GA_launcher::process_one_cell ()
{
	// Determine cell number:
	bool found_incomplete_cell = false;
	size_t cell_x = 0, cell_y = 0;

	for (size_t x = 0; x < zones_x; ++x) {
		for (size_t y = 0; y < zones_y; ++y) {
			if (!workers_ready[x][y]) {
				found_incomplete_cell = true;
				cell_x = x;
				cell_y = y;
				break;
			}
		}
		if (found_incomplete_cell) break;
	}


	if (!found_incomplete_cell) {
		std::cout << "[multizone_GA_launcher]: Didn't find any non-processed cells => finishing..." << std::endl;
		return false;
	}

	std::cout << "_____________________________________________" << std::endl;
	std::cout << "[multizone_GA_launcher]: processing new cell: x = " << cell_x << ", y = " << cell_y
		<< ": " << zones.zone_descriptor.get_2d_cells()[cell_x][cell_y]
	<< std::endl;

	auto& worker = workers[cell_x][cell_y];
	worker->run_remaining_iterations();

	worker->print_diagnostic_information();
	std::cout << "_____________________________________________" << std::endl;

	workers_ready[cell_x][cell_y] = true;

	return true;
}


void multizone_GA_launcher::run ()
{
	while(process_one_cell()) {}
}

std::vector<double> multizone_GA_launcher::glue_best_genomes ()
{
	std::vector<double> res;
	size_t total_buffer_size = zones_x * zones_y * workers[0][0]->get_best_genome().size();

	std::cout << "Copying " << total_buffer_size << " doubles" << std::endl;

	res.reserve(total_buffer_size);

	size_t x_index = 0;
	for(auto& col : workers) {
		size_t y_index = 0;
		for (auto& worker : col) {
			const auto& this_cell_descriptor = zones.zone_descriptor.get_2d_cells()[x_index][y_index];

			size_t this_x_shift = this_cell_descriptor.min_x;
			size_t this_y_shift = this_cell_descriptor.min_y;

			auto strokes = unpack_stroke_data_buffer(worker->get_best_genome());

			std::for_each(strokes.begin(), strokes.end(),
			              [this_x_shift, this_y_shift](stroke& stroke){
				              shift_stroke(stroke, { double(this_x_shift), double(this_y_shift) });
			              });

			auto shifted_buffer = pack_stroke_data(strokes);

			std::copy(shifted_buffer.begin(), shifted_buffer.end(), std::back_inserter(res));

			y_index++;
		}
		x_index++;
	}

	assert(res.size() == total_buffer_size);

	return res;
}


Image multizone_GA_launcher::get_resultant_image ()
{
	return stroke_buffer_to_image(glue_best_genomes(), image);
}

void multizone_GA_launcher::save_result (const fs::path& filename)
{
	save_image(get_resultant_image(), filename);
}






