#include "algorithm_statistics.h"
#include "utils/logger.h"

AlgorithmStatistics AlgorithmStatistics::operator+ (const AlgorithmStatistics& other) const
{
  return AlgorithmStatistics {
          computations_performed + other.computations_performed,
          pixels_processed + other.pixels_processed,
          time_spent_computing + other.time_spent_computing,
  };
}

void AlgorithmStatistics::print_diagnostic_information () const
{
  size_t exp_precision = 10;

  LogConsoleInfo("Statistics") << "Computations performed: " << computations_performed;
  LogConsoleInfo("Statistics") << "Average computational time: " << average_computational_time() * 1e+3 << "ms";
  LogConsoleInfo("Statistics") << "Computational time per pixel: " << computational_time_per_pixel() * 1e+9 << "ns";
  LogConsoleInfo("Statistics") << "Computational speed: " << std::round(pixels_per_second() / 1e+6 * exp_precision) / exp_precision << " MegaPixel / sec";
}
