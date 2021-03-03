//
// Created by Vova on 02.10.2020.
//

#include "launch_main.h"
#include "launching/single_zone_workers/GA_worker.h"

#include "io_api/image_io_utils.h"
#include "GA_parameter_sets.h"
#include "common_operations/image_adaptive_params.h"

static inline void save_log_json(std::vector<colored_stroke> strokes,
                                 fs::path filepath = fs::path(painter_base_path) / "log" / "latest" / "result.json")
{
    std::vector<color> pallete; // TODO: use specific color for strokes
    for (auto& col_stroke : strokes) {
        if (std::find(pallete.begin(), pallete.end(), col_stroke.background_color) == std::end(pallete)) {
            pallete.emplace_back(col_stroke.background_color);
        }
    }

    json j;
    to_json(j, strokes, pallete);
    std::ofstream json_file(filepath);
    json_file << j;
    json_file.close();
}

void launch_single_zone_GA (const std::string &filename)
{
	Image image = open_image(filename);

	CommonStrokingParams this_common_params = default_stroking_parameters;
	GA_stroking_parameters this_ga_params = default_GA_params;

	GA_worker worker(image, this_common_params, this_ga_params);
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
	CommonStrokingParams this_common_params = default_stroking_parameters;
	GA_stroking_parameters this_ga_params = default_GA_params;
	image_splitting_params this_splitting_params = van_gogh_splitting_params;

	Image image = open_image(filename);
	MultizoneLaunchWrapper<GA_worker> launcher(image,
	                                this_splitting_params.zones_x, this_splitting_params.zones_y, this_splitting_params.overlay_percent,
	                                this_common_params, this_ga_params);
	std::cout << "[main launching function]: Performed initialization. Running.." << std::endl;

	launcher.run();

	launcher.save_result(painter_base_path / "log" / "latest" / "result.png");
}


void launch_single_zone_annealing(const std::string& filename) {
    std::cout << "Launching single zone annealing…" << std::endl;

	Image image = open_image(filename);
	std::cout << "[launch_single_zone_annealing]: Opened image at " << filename << std::endl;

    CommonStrokingParams this_common_params = default_stroking_parameters;
    AnnealingStrokingParams this_annealing_params = default_annealing_params;

    AnnealingWorker worker(image, this_common_params, this_annealing_params);
    std::cout << "[launch_single_zone_annealing]: initialized AnnealingWorker => Running the iterations…" << std::endl;
    worker.run_remaining_iterations();
    std::cout << "[launch_single_zone_annealing]: Have run all the Annealing iterations, just saving the result…" << std::endl;

    auto strokes = unpack_stroke_data_buffer(worker.get_best_genome());
    colorize_strokes(strokes, image); // TODO: Use specific color of image

    Image result = make_default_image(image.cols, image.rows);
    rasterize_strokes(result, strokes);
    save_image(result, (fs::path(painter_base_path) / "log" / "latest" / "result.png").string());

    save_log_json(strokes);

    std::cout << "[launch_single_zone_annealing]: Saved everything, showing results and diagnostics…" << std::endl;

    show_image_in_system_viewer(result);

    worker.print_diagnostic_information();
    worker.show_fitness_dynamic();
}


void launch_multizone_annealing(const std::string& filename) {
	// TODO
}


void launch_svg_zonized_stroking()
{
	fs::remove_all(painter_base_path / "log" / "latest");


}


void launch_svg_stroking(const std::string &filename) {
    fs::remove_all(painter_base_path / "log" / "latest");

    SVG_service service(filename);
    service.split_paths();

    Image original = service.get_raster_original_image();
    save_image(original, (fs::path(painter_base_path) / "log" / "latest" / "original.png").string());

    CommonStrokingParams common_params = switch_to_absolute_values(default_stroking_parameters,
                                                                   original.cols, original.rows);
    GA_stroking_parameters ga_params = default_GA_params;
    cv::Size original_size(original.rows, original.cols);

    Image image;
    std::vector<colored_stroke> strokes;
    /// Main stroking loop, add new strokes at each iteration:
    while (service.load_current_image(image))
    {
        color cur_color = service.get_current_color();

        auto cur_params = common_params;
        cur_params.stroke_number = calc_strokes_count(image, original_size, common_params.stroke_number);
        cur_params.use_constant_color = true;
        cur_params.stroke_color = cur_color;

        std::cout << "[Launch] Run #" << (service.get_it() + 1) << " simulation ("
                    << cur_params.stroke_number << " strokes, " << cur_color << " color)" << std::endl;
        GA_worker worker(image, cur_params, ga_params,
                         fs::path(painter_base_path) / "log" / "latest" / ("part" + std::to_string(service.get_it())));
        worker.run_remaining_iterations();

        auto cur_strokes = unpack_stroke_data_buffer(worker.get_best_genome());
        colorize_strokes(cur_strokes, cur_color);
        service.shift_strokes(cur_strokes);
        strokes.insert(strokes.end(), cur_strokes.begin(), cur_strokes.end());

        service.next();
    }

    // View result
    Image result = make_default_image(original.cols, original.rows);
    rasterize_strokes(result, strokes);
    save_image(result, (fs::path(painter_base_path) / "log" / "latest" / "result.png").string());
    std::cout << "[Launch] Result: " << strokes.size() << " strokes." << std::endl;

    // Save strokes
    save_log_json(strokes);

    show_image_in_system_viewer(result);
}





