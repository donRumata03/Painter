//
// Created by Vova on 05.10.2020.
//

#pragma once

#include "painter_pch.h"

#include <GA_operations/util.h>
#include <rasterization/stroke_rasterizer.h>

#include <common_operations/filesystem_primitives.h>

/**
 * Renders and logs all the intermediate states as images to folders to a given one.
 * Does this and copies all the generated folder to a new one, specially for generated  in destructor
 */
class image_logging_callback
{
    ImageStrokingData imageData;

	fs::path path_for_logging;
	fs::path path_for_best_genomes;

	bool enable_console_output = true;
	bool detalized_logging = false;
	double logged_genome_rate = 1.;


//	std::string new_population_logging_path;
//	std::string after_mutation_logging_path;
//	std::string after_constraint_logging_path;

public:
	image_logging_callback() = default;
	image_logging_callback(ImageStrokingData imageData,
                        const std::string& path_for_logging,
                        double logged_percent,
                        bool log_precisely = true,
                        bool enable_console_output = true)
										: path_for_logging(path_for_logging),
										imageData(std::move(imageData)),
										logged_genome_rate(logged_percent),
										detalized_logging(log_precisely),
                                        enable_console_output(enable_console_output)
	{
		ensure_log_cleared(this->path_for_logging);

		save_image(this->imageData.image, this->path_for_logging / "original.png");

		path_for_best_genomes = this->path_for_logging / "best_genomes";
		fs::create_directories(path_for_best_genomes);

		// std::cout << "Initialized logger with path to best genomes: " << path_for_best_genomes << std::endl;
	}


	void operator ()(const GA::Population & population, size_t epoch_index, logging_type this_logging_type) {

		if (this_logging_type == logging_type::best_genome) {
			assert(population.size() == 1);
			fs::path filename = path_for_best_genomes / (std::to_string(epoch_index) + ".png");
			if (enable_console_output) std::cout << "Saving best genome image to " << filename.string() << std::endl;
			save_stroke_buffer_as_image(population[0], imageData, filename);

			return;
		}

		/// Determine operation:
		std::string this_operation_name;
		auto operation_index = size_t(-1);
		switch(this_logging_type) {
			case logging_type::new_epoch:
				this_operation_name = "new_epoch";
				operation_index = 1;
                if (enable_console_output) std::cout << "[logger]: Generated new population..." << std::endl;
				break;
			case logging_type::after_mutation:
				this_operation_name = "after_mutation";
				operation_index = 2;
                if (enable_console_output) std::cout << "[logger]: Mutated..." << std::endl;
				break;
			case logging_type::after_constraining:
				this_operation_name = "after_constraining";
				operation_index = 3;
                if (enable_console_output) std::cout << "[logger]: Constrained..." << std::endl;
				break;
			default:
				assert(false);
		}

		if (!detalized_logging) return;

		/// Render and save images:
		fs::path this_root_path = path_for_logging / std::to_string(epoch_index);

		fs::path this_path_by_operation = this_root_path / this_operation_name;
		fs::path this_path_by_index = this_root_path / "by_index";

		fs::create_directories(this_path_by_operation);
		fs::create_directories(this_path_by_index);

		auto genomes_to_log = size_t(std::round(population.size() * this->logged_genome_rate));
        if (enable_console_output) std::cout << "[Image Logger]: logging " << genomes_to_log << "genomes" << std::endl;

		size_t genome_index = 0;
		for (auto& genome : population) {
			if (genome_index >= genomes_to_log) break;

			fs::path final_folder_by_index = this_path_by_index / (std::to_string(genome_index));
			fs::create_directories(final_folder_by_index);

			fs::path this_file_by_index = final_folder_by_index / (std::to_string(operation_index) + "." + this_operation_name + ".png");
			fs::path this_file_by_operation = this_path_by_operation / (std::to_string(genome_index) + ".png");
            if (enable_console_output) std::cout << "Saving to: " << this_file_by_operation << " && " << this_file_by_index << std::endl;
			save_stroke_buffer_as_images(genome, imageData, { this_file_by_index, this_file_by_operation });

			genome_index++;
		}
	}

	~image_logging_callback() = default;
};


