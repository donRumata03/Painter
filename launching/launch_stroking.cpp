//
// Created by Vova on 02.10.2020.
//

#include "launch_stroking.h"

#include "io_api/image_io_utils.h"
#include "GA_parameter_sets.h"
#include "common_operations/image_adaptive_params.h"
#include "data_representation/paint_plan.h"
#include "utils/Progress.h"
#include <common_operations/image_color_deduction.h>
#include <optimization/contrast_color_finding.h>
#include "utils/Logger.h"


static inline void save_log_json(const std::vector<colored_stroke>& strokes,
                                 const fs::path& filepath = fs::path(painter_base_path) / "log" / "latest" / "plan.json")
{
    json j = PaintPlan(strokes);
    std::ofstream json_file(filepath);
    json_file << j.dump(1, '\t');
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

	worker.get_efficiency_account().print_diagnostic_information();
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


void launch_single_zone_annealing(const std::string& filename, bool auto_find_color) {
    std::cout << "Launching single zone annealing…" << std::endl;

	Image image = open_image(filename);
	std::cout << "[launch_single_zone_annealing]: Opened image at " << filename << std::endl;

	CommonStrokingParams this_common_params = default_stroking_parameters;

	color major_color {};
	color canvas_color {};
	color background_color {};
    if (auto_find_color) {
	    major_color = find_major_image_color(image);
	    canvas_color = find_contrast_color(major_color);
		background_color = find_image_background_color(image);

	    std::cout << "[launch_single_zone_annealing]: Found major color for the image: "
	              << major_color << " the one, contrast to it, is " << canvas_color << ". It's taken to be canvas color" << std::endl;
		std::cout << "[launch_single_zone_annealing]: Original Image's background color is " << background_color << std::endl;

		this_common_params.stroke_color = major_color;
	    // this_common_params.canvas_color = canvas_color;
	    this_common_params.use_constant_color = true;
    }

	AnnealingStrokingParams this_annealing_params = default_annealing_params;


    AnnealingWorker worker(image, this_common_params, this_annealing_params);
    std::cout << "[launch_single_zone_annealing]: initialized AnnealingWorker => Running the iterations…" << std::endl;
    worker.run_remaining_iterations();
    std::cout << "[launch_single_zone_annealing]: Have run all the Annealing iterations, just saving the result…" << std::endl;

    auto strokes = unpack_stroke_data_buffer(worker.get_best_genome());
    colorize_strokes(strokes, ImageStrokingData(image, this_common_params.use_constant_color, this_common_params.stroke_color));

    Image result = make_default_image(image.cols, image.rows, canvas_color);
    rasterize_strokes(result, strokes);
    save_image(result, (fs::path(painter_base_path) / "log" / "latest" / "result.png").string());

    save_log_json(strokes);

    std::cout << "[launch_single_zone_annealing]: Saved everything, showing results and diagnostics…" << std::endl;

    show_image_in_system_viewer(result);

    worker.get_efficiency_account().print_diagnostic_information();
    worker.show_fitness_dynamic();
}


void launch_multizone_annealing(const std::string& filename) {
	// TODO
}


template<class WorkerType>
void launch_svg_zone_stroking(const std::string& filename, const Canvas& canvas)
{
    LogConsoleInfo("Launch") << "Launching SVG zoned stroking…";

    CommonStrokingParams common_params = default_stroking_parameters;
    //typename WorkerType::ParametersType spec_params = get_default_special_params<WorkerType>();
    std::optional<typename WorkerType::ParametersType> spec_params = get_default_special_params<WorkerType>();

    SvgZoneLauncher<WorkerType> launcher(filename, common_params, spec_params.value(), canvas, not CanBeParallelized<WorkerType>::value);

    launcher.run();

    // View result
    // Full size, only result
    auto size = launcher.get_image_size();
    Image result = make_default_image(size.width, size.height);
    auto strokes = launcher.get_final_strokes();
    rasterize_strokes(result, strokes);
    save_image(result, (fs::path(painter_base_path) / "log" / "latest" / "result.png").string());
    LogConsoleSuccess("Launch") << "Result: " << strokes.size() << " strokes";

    // Full size, result on canvas (in px)
    Image result_mm = make_default_image(canvas.width(), canvas.height());
    rasterize_strokes(result_mm, launcher.get_final_strokes(Units::PX, true));
    save_image(result_mm, (fs::path(painter_base_path) / "log" / "latest" / "result_canvas.png").string());

    // Save strokes
    save_log_json(launcher.get_final_strokes(Units::MM, true));
    save_log_json(launcher.get_final_strokes(Units::PX, true),
                  fs::path(painter_base_path) / "log" / "latest" / "plan_px.json");

    show_image_in_system_viewer(result);
}

void launch_svg_stroking(const std::string &filename) {
	ensure_log_cleared();

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
    Progress progress(service.get_shapes_count());
    /// Main stroking loop, add new strokes at each iteration:
    while (service.load_current_image(image))
    {
        color cur_color = service.get_current_color();

        auto cur_params = common_params;
        // cur_params.canvas_color = find_contrast_color(cur_color); // TODO: apply this string
        cur_params.stroke_number = calc_strokes_count(image, original_size, common_params.stroke_number, cur_params.canvas_color);
        cur_params.use_constant_color = true;
        cur_params.stroke_color = cur_color;

        /*std::cout << "[Launch] Run #" << (service.get_it() + 1) << " simulation ("
                    << cur_params.stroke_number << " strokes, " << cur_color << " color)" << std::endl;*/
        GA_worker worker(image, cur_params, ga_params,
                         fs::path(painter_base_path) / "log" / "latest" / ("part" + std::to_string(service.get_it())),
                         false);
        worker.run_remaining_iterations();

        auto cur_strokes = unpack_stroke_data_buffer(worker.get_best_genome());
        colorize_strokes(cur_strokes, cur_color);
	    service.shift_strokes_to_current_box(cur_strokes);
        strokes.insert(strokes.end(), cur_strokes.begin(), cur_strokes.end());

        progress.update();
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

template void launch_svg_zone_stroking<GA_worker>(const std::string& filename, const Canvas& canvas);
template void launch_svg_zone_stroking<AnnealingWorker>(const std::string& filename, const Canvas& canvas);
