#include <gtest/gtest.h>

#include "operations/stroke/generation.h"


TEST(GenerationTest, StrokeGeneration) {
  StrokeLimits limits{.min_dx = 10, .max_dx = 20, .min_dy = 10, .max_dy = 20, .min_width = 4, .max_width = 5,
          .image_rectangle = {0., 100., 0., 100.}};
  std::cout << limits << std::endl << std::endl;

  for (size_t i = 0; i < 100; i++) {
    auto stroke = generate_stroke(limits);

    EXPECT_TRUE(limits.stroke_satisfies_requirements(stroke)) << "[" << std::setw(3) << i << "] " << stroke;
  }
}