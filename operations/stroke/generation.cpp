#include "generation.h"


Stroke generate_stroke(const StrokeLimits& limits) {
  double image_dx = limits.image_rectangle.max_x - limits.image_rectangle.min_x;
  double image_dy = limits.image_rectangle.max_y - limits.image_rectangle.min_y;

  if (!limits.validate_self()) {
    LogConsoleError("Stroke Generator") << "Invalid limits: " << limits;
    assert(false);
  }

  std::random_device rd{};
  std::mt19937 gen{rd()};

  Stroke res_stroke;

  // Firstly generate the stroke without its position
  // (all the three points are from <(0, limits.min_x); (0, limits.min_y)>)
  auto gen_x = [&]() { return randval(0, limits.max_dx, gen); };
  auto gen_y = [&]() { return randval(0, limits.max_dy, gen); };

  res_stroke.p0.x = gen_x();
  res_stroke.p0.y = gen_y();

  res_stroke.p1.x = gen_x();
  res_stroke.p1.y = gen_y();

  res_stroke.p2.x = gen_x();
  res_stroke.p2.y = gen_y();

  // Find its bounding box
  auto bounding_box = res_stroke.get_curve_bounding_box();
  double max_shift_x = image_dx - bounding_box.dx();
  double max_shift_y = image_dy - bounding_box.dy();


  // Generate positioning
  double shift_x = limits.image_rectangle.min_x + randval(0, max_shift_x, gen);
  double shift_y = limits.image_rectangle.min_y + randval(0, max_shift_y, gen);

  // Shift the stroke
  shift_stroke(res_stroke, {shift_x, shift_y});

  // Width is easy to generate
  res_stroke.width = randval(limits.min_width, limits.max_width, gen);

  constrain_stroke_size_parameters(res_stroke, limits.min_dx, limits.max_dx, limits.min_dy, limits.max_dy,
                                   limits.min_width, limits.max_width);

  return res_stroke;
}
