#include <gtest/gtest.h>

#include "operations/stroke/generation.h"


TEST(GenerationTest, StrokeGeneration) {
  StrokeLimits limits{.min_sum = 10, .max_dx = 20, .max_dy = 20, .min_width = 4, .max_width = 5,
          .image_rectangle = {0., 100., 0., 100.}};
  std::cout << limits << std::endl << std::endl;

  for (size_t i = 0; i < 100; i++) {
    auto stroke = generate_stroke(limits);
    auto test = limits.stroke_satisfies_requirements(stroke);
    std::string type;

    switch (test) {
      case BadWidth: type = "BadWidth"; break;
      case BadCurvature: type = "BadCurvature"; break;
      case NotFitIn: type = "NotFitIn"; break;
      case BoxNotFitIn: type = "BoxNotFitIn"; break;
      default: break;
    }

    RangeRectangle<double> stroke_bounding_box = stroke.get_curve_bounding_box();

    EXPECT_TRUE(test == StrokeLimitsRequirementsResult::Satisfied)
      << "[" << std::setw(3) << i << "] (" << type << ") " << stroke << std::endl << stroke_bounding_box << std::endl
      << "dx: " << stroke_bounding_box.dx() << ", dy: " << stroke_bounding_box.dy() << ", sum: "
      << (stroke_bounding_box.dx() + stroke_bounding_box.dy());
  }
}