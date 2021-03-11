//
// Created by Vova on 26.02.2021.
//

#include "on_improvement_updating_logger.h"


void OnImprovementUpdatingLogger::operator() (const std::vector<double>& stroke_set, size_t this_iteration, double error) const
{
	// As for now, just have some probability of showing:
	if (not (pythonic_random() < logging_fraction)) {
		return;
	}

	fs::path filename = updating_path / (std::to_string(this_iteration) + ".png");
	if (enable_console_output) std::cout << "Update logged:  " << filename.string() << std::endl;
	save_stroke_buffer_as_image(stroke_set, imageData, filename);
}
