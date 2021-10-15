#pragma once

#include "data/color.h"
#include "data/stroke.h"
#include "rasterization/stroke.h"
#include "operations/color/optimization.h"
#include "utils/image.h"


struct ImageStrokingData {
 public:
  ImageStrokingData() = default;

  ImageStrokingData(Image image, bool use_constant_color = false, const Color& stroke_color = {1., 1., 1.})
          : image(std::move(image)), use_constant_color(use_constant_color), stroke_color(stroke_color) {}

  Image image;

  bool use_constant_color = false;
  Color stroke_color{};

};

double smooth_transmit_between_borders_linearal(double max_dimensions);

double smooth_transmit_between_borders_arctan(double max_dimensions,
                                              double max_width);

double smooth_transmit_between_borders_sigmoid(double max_dimensions, double max_width);

double fatness_to_width(const Stroke& stroke, const double fatness);

/**
 * @attention The returning buffer actually doesn`t have colors. The strokes are marked as colored for performance reasons.
 */
std::vector<ColoredStroke> unpack_stroke_data_buffer(const std::vector<double>& buffer);

void colorize_strokes(std::vector<ColoredStroke>& strokes, const Image& image);

void colorize_strokes(std::vector<ColoredStroke>& strokes, const Color& setup_color);

void colorize_strokes(std::vector<ColoredStroke>& strokes_to_colorize, const ImageStrokingData& data);

double width_to_fatness(const Stroke& stroke, double width);

template <class StrokeType,
        std::enable_if_t<std::is_same_v<StrokeType, Stroke> or
                         std::is_same_v<StrokeType, ColoredStroke>, int *> nothing = nullptr>
inline std::vector<double> pack_stroke_data(const std::vector<StrokeType>& strokes) {
  auto total = strokes.size();
  std::vector<double> stroke_data_buffer(total * 7);

  for (size_t i = 0; i < total; ++i) {
    size_t offset = i * 7;

    stroke_data_buffer[offset] = strokes[i].p0.x;
    stroke_data_buffer[offset + 1] = strokes[i].p0.y;

    stroke_data_buffer[offset + 2] = strokes[i].p1.x;
    stroke_data_buffer[offset + 3] = strokes[i].p1.y;

    stroke_data_buffer[offset + 4] = strokes[i].p2.x;
    stroke_data_buffer[offset + 5] = strokes[i].p2.y;

    stroke_data_buffer[offset + 6] = width_to_fatness(strokes[i], strokes[i].width);
  }

  return stroke_data_buffer;
}

Image stroke_buffer_to_image(const std::vector<double>& stroke_buffer, const ImageStrokingData& data,
                             Color canvas_color = {0., 0., 0.,});

void save_stroke_buffer_as_image(const std::vector<double>& stroke_buffer, const ImageStrokingData& data,
                                 const fs::path& filename, Color canvas_color);


void save_stroke_buffer_as_images(const std::vector<double>& stroke_buffer, const ImageStrokingData& data,
                                  const std::vector<fs::path>& filenames, Color canvas_color);


double transfer_range(double value, std::pair<double, double> from_range, std::pair<double, double> to_range);

