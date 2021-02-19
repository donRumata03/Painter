//
// Created by Vova on 02.10.2020.
//

#include "launch_main_ga.h"
#include "launching/single_zone_workers/GA_worker.h"

#include "io_api/image_io_utils.h"
#include "GA_parameter_sets.h"

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
	// TODO
}


void launch_multizone_annealing(const std::string& filename) {
	// TODO
}



void launch_svg_stroking(const std::string &filename) {
    fs::remove_all(painter_base_path / "log" / "latest");

    SVG_service service(filename);
    service.split_paths();

    Image image;
    std::vector<colored_stroke> strokes;
    while (service.load_current_image(image))
    {
	    CommonStrokingParams this_common_params = default_stroking_parameters;
	    GA_stroking_parameters this_ga_params = default_GA_params;

        GA_worker worker(image, this_common_params, this_ga_params,
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


    show_image_in_system_viewer(image);

    // Debug show
//    Image result = image.clone();
//    convert_image_between_RGB_and_BGR<Pixel>(result);
//    cv::imshow("result", result);
//    cv::waitKey(0);
}





