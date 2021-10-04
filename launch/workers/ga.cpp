#include "ga.h"


GaWorker::GaWorker(const Image& image, const CommonStrokingParams& stroking_params,
                   const GaStrokingParams& optimizer_params, const fs::path& logging_path,
                   bool verbose)
        : stroking_params(stroking_params), GA_params(optimizer_params), SimpleWorker(logging_path, verbose) {
  image.copyTo(initial_image);

  image_w = image.cols;
  image_h = image.rows;

  // Count typical distances
  limits = generate_stroke_limits_by_raw_parameters(stroking_params, image_w, image_h);

  // GA data
  point_ranges = generate_point_ranges_by_raw_parameters(stroking_params, image_w, image_h);
  mutation_sigmas = generate_point_sigmas_by_raw_parameters(stroking_params, image_w, image_h);

  // Init GA operation performers
  constrainer = GaConstrainer(limits);
  generator = GaPopulationGenerator(limits, stroking_params.stroke_number);
  crossover = GaCrossover();
  mutator = GaMutator(limits, stroking_params.move_mutation_probability);

  ga_operations.genome_constraint = constrainer;
  ga_operations.population_generation = generator;
  ga_operations.parents_matting = crossover;
  ga_operations.mutation = mutator;

  fitness_function = FitnessFunction(
          ImageStrokingData(image, stroking_params.use_constant_color, stroking_params.stroke_color),
          stroking_params.stroke_number,
          !GA_params.allow_multithreading,
          false,
          stroking_params.canvas_color
  );

  logger = GaLoggingCallback(ImageStrokingData(image, stroking_params.use_constant_color, stroking_params.stroke_color),
                             optimizer_params.epoch_num,
                             logging_path.string(),
                             stroking_params.logging_percentage,
                             verbose,
                             (stroking_params.logging_percentage != 0),
                             stroking_params.canvas_color);


  // GA params
  ga_params = {
          GA_params.population_size,
          /// numeric params

          GA::hazing_GA_params{
                  .hazing_percent = 0.8,
          },
          GA::mutation_GA_params{
                  .mutation_percent_sigma = -1,
                  .target_gene_mutation_number = stroking_params.stroke_number * 4., // Out of `stroke_number * 7`
                  .cut_mutations = true,
                  .individual_mutation_sigmas = mutation_sigmas,
          },
          GA::crossover_mode::low_variance_genetic,
          std::optional<double>{},
          GA::threading_GA_params{
                  .allow_multithreading = GA_params.allow_multithreading,
                  .threads = std::thread::hardware_concurrency() - 2
          },

          ga_operations,

          GA::exception_policy::catch_and_log_fact,
  };

  optimizer.emplace(fitness_function, point_ranges, ga_params);
  optimizer->plug_logger(logger);
  optimizer->set_informer(GaInformer(image, GA_params.epoch_num, verbose));

  LogSuccess("GA Worker") << "Inited";
}

void GaWorker::set_basic_strokes(const std::vector<Stroke>& strokes) {
  generator = GaPopulationGenerator(strokes);
}

void GaWorker::run_one_iteration() {
  optimizer->run_one_iteration(GA_params.epoch_num);
}

void GaWorker::run_remaining_iterations() {
  optimizer->run_many_iterations(
          GA_params.epoch_num - optimizer->iterations_processed(),
          GA_params.epoch_num
  );
}

void GaWorker::save_fitness_dynamic() {
  auto fhist = optimizer->get_fitness_history();
  LogConsoleInfo("GA Worker") << "Fitness dynamic: " << fhist;

  std::vector<double> xs_for_fitnesses(fhist.size());
  for (size_t x_index = 0; x_index < xs_for_fitnesses.size(); ++x_index) {
    xs_for_fitnesses[x_index] = double(x_index);
  }
  add_vectors_to_plot(xs_for_fitnesses, fhist);
  show_plot({.output_filename = (logging_path / "fitness_plot.png").string()});
}

const std::vector<double>& GaWorker::get_best_genome() {
  return optimizer->get_best_genome();
}


