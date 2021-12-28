#include <gtest/gtest.h>

#include "operations/stroke/generation.h"
#include "operations/stroke/sorting.h"


TEST(SortingTest, SortStrokes) {
  // TODO: upgrade to fixture with mocking
  const size_t n = 100, w = 1000, h = 1000, zones = 3, colors_n = 10;
  StrokeLimits limits{.min_sum = 10, .max_dx = 20, .max_dy = 20, .min_width = 4, .max_width = 5,
          .image_rectangle = {0., w, 0., h}};
  std::random_device rd{};
  std::mt19937 gen{rd()};

  std::vector<Color> colors(colors_n);
  for (auto& color : colors) {
    color = Color(randval(0., 1., gen), randval(0., 1., gen), randval(0., 1., gen));
    std::cout << color << std::endl;
  }

  std::vector<ColoredStroke> strokes(n);
  for (auto& stroke : strokes) {
    stroke = ColoredStroke(generate_stroke(limits), colors[randint(0, colors_n - 1, gen)]);
  }

  strokes = sort_strokes(strokes, w, h, zones);

  auto check = [&](const ColoredStroke& s1, const ColoredStroke& s2) {
    if (s1.background_color != s2.background_color) {
      return s1.background_color < s2.background_color;
    }

    auto zone_finder = [&](point p) -> std::pair<size_t, size_t> {
      return {size_t(std::round(p.y / ((double) h / zones))), size_t(std::round(p.x / ((double) w / zones)))};
    };

    return zone_finder(s1.p0) <= zone_finder(s2.p0);
  };

  for (size_t i = 0; i < n - 1; i++) {
    EXPECT_TRUE(check(strokes[i], strokes[i + 1]))
                  << std::fixed << std::setprecision(3) << strokes[i] << " | " << strokes[i + 1];
  }
}