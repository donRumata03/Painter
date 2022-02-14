#include "annealing.h"


AnnealingWorker::AnnealingWorker(const Image& image, const CommonStrokingParams& stroking_params,
                                 const AnnealingStrokingParams& params, const fs::path& logging_path,
                                 bool verbose)

        : annealing_stroking_params(params), common_stroking_params(stroking_params),
          SimpleWorker(logging_path, verbose) {
  image.copyTo(initial_image);

  image_w = initial_image.cols;
  image_h = initial_image.rows;

  error_function = FitnessFunction(
          ImageStrokingData(image, stroking_params.use_constant_color, stroking_params.stroke_color),
          common_stroking_params.stroke_number,
          true, // annealing is sequential
          common_stroking_params.canvas_color,
          common_stroking_params.imposition_penalization_coefficient,
          false // raw MSE
  );

  auto stroke_limits = generate_stroke_limits_by_raw_parameters(stroking_params, image_w, image_h);

  mutator = AnnealingGenomeMutator(
          stroke_limits,
          generate_point_sigmas_by_raw_parameters(stroking_params, image_w, image_h),
          common_stroking_params.stroke_number * double(sizeof(Stroke) / sizeof(double)) *
          annealing_stroking_params.gene_mutation_fraction,
          common_stroking_params.move_mutation_probability,
          true // It will constrain the strokes before returning result
  );

  generator = AnnealingGenomeGenerator(stroke_limits, common_stroking_params.stroke_number);

  logger = AnnealingLoggingCallback(
          ImageStrokingData(image, stroking_params.use_constant_color, stroking_params.stroke_color),
          params.iterations, stroking_params.logging_percentage, logging_path, stroking_params.canvas_color,
          verbose);
}

void AnnealingWorker::set_basic_strokes(const std::vector<Stroke>& strokes) {
  generator = AnnealingGenomeGenerator(strokes);
}

void AnnealingWorker::run_one_iteration() {
  throw std::runtime_error("Can't run only one iteration for annealing!");
}

void AnnealingWorker::run_remaining_iterations() {
  result = annealing_optimize<double, void>( // TODO: develop fast fitness function recounting
          error_function,
          AnnealingOptimizeParameters{
                  .iterations = annealing_stroking_params.iterations,
                  .exiting_value = 0.,
                  .typical_temperature = annealing_stroking_params.typical_temperature,
                  .genes_in_genome = common_stroking_params.stroke_number * (sizeof(Stroke) / sizeof(double)),
          },
          generator,
          mutator,
          custom_exp_temperature_dynamic(0.2),
          verbose,
          nullptr,
          nullptr,
          nullptr,
          logger
  );
}

const std::vector<double>& AnnealingWorker::get_best_genome() {
  return result.best_genome;
}

void AnnealingWorker::save_fitness_dynamic() {
  auto current_ys = result.current_energy_dynamic;
  auto best_ys = result.best_energy_dynamic;

  std::vector<double> xs_to_plot(current_ys.size());
  std::iota(xs_to_plot.begin(), xs_to_plot.end(), 0.);

  add_vectors_to_plot(xs_to_plot, current_ys, {.name = "MSE Dynamic of current genome"});
  add_vectors_to_plot(xs_to_plot, best_ys, {.name = "MSE Dynamic of best genome since the beginning"});
  show_plot({.log_y = true, .output_filename = (logging_path / "fitness_plot.png").string()});
}
