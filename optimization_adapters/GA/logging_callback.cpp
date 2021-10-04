#include "logging_callback.h"


GaLoggingCallback::GaLoggingCallback(ImageStrokingData imageData, size_t total_epoch,
                                     const std::string& path_for_logging, double logged_percent, bool verbose,
                                     bool log_precisely, Color canvas_color)
        : path_for_logging(path_for_logging), imageData(std::move(imageData)), canvas_color(canvas_color),
          logged_genome_rate(logged_percent), verbose(verbose), detalized_logging(log_precisely) {
  ensure_log_cleared(this->path_for_logging);

  save_image(this->imageData.image, this->path_for_logging / "original.png");

  path_for_best_genomes = this->path_for_logging / "best_genomes";
  fs::create_directories(path_for_best_genomes);

  LogInfo("GA", "Log") << "Initialized logger with path to best genomes: " << path_for_best_genomes;
  if (verbose) Logger::NewProgress(total_epoch);
}

void GaLoggingCallback::operator()(const GA::Population& population, size_t epoch_index,
                                   logging_type type) {
  if (type == logging_type::best_genome) {
    assert(population.size() == 1);
    fs::path filename = path_for_best_genomes / (std::to_string(epoch_index) + ".png");
    LogInfo("GA", "Log") << "Saving best genome image to " << filename.string();
    save_stroke_buffer_as_image(population[0], imageData, filename, canvas_color);

    if (verbose) Logger::UpdateProgress();

    return;
  }

  // Determine operation
  std::string this_operation_name;
  auto operation_index = size_t(-1);
  switch (type) {
    case logging_type::new_epoch:
      this_operation_name = "new_epoch";
      operation_index = 1;
      if (verbose) LogInfo("GA", "Log") << "Generated new population...";
      break;
    case logging_type::after_mutation:
      this_operation_name = "after_mutation";
      operation_index = 2;
      if (verbose) LogInfo("GA", "Log") << "Mutated...";
      break;
    case logging_type::after_constraining:
      this_operation_name = "after_constraining";
      operation_index = 3;
      if (verbose) LogInfo("GA", "Log") << "Constrained...";
      break;
    default:
      assert(false);
  }


  if (!detalized_logging) return;

  // Render and save images
  fs::path this_root_path = path_for_logging / std::to_string(epoch_index);

  fs::path this_path_by_operation = this_root_path / this_operation_name;
  fs::path this_path_by_index = this_root_path / "by_index";

  fs::create_directories(this_path_by_operation);
  fs::create_directories(this_path_by_index);

  auto genomes_to_log = size_t(std::round(population.size() * this->logged_genome_rate));
  if (verbose) LogInfo("GA", "Log") << "Logging " << genomes_to_log << "genomes";

  size_t genome_index = 0;
  for (auto& genome : population) {
    if (genome_index >= genomes_to_log) break;

    fs::path final_folder_by_index = this_path_by_index / (std::to_string(genome_index));
    fs::create_directories(final_folder_by_index);

    fs::path this_file_by_index =
            final_folder_by_index / (std::to_string(operation_index) + "." + this_operation_name + ".png");
    fs::path this_file_by_operation = this_path_by_operation / (std::to_string(genome_index) + ".png");
    if (verbose) {
      LogInfo("GA", "Log") << "Saving to: " << this_file_by_operation << " && " << this_file_by_index;
    }
    save_stroke_buffer_as_images(genome, imageData, {this_file_by_index, this_file_by_operation}, canvas_color);

    genome_index++;
  }
}