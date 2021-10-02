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


/**
 * @attention The returning buffer actually doesn`t have colors. The strokes are marked as colored for performance reasons.
 */
inline std::vector<ColoredStroke> unpack_stroke_data_buffer(const std::vector<double>& buffer) {
  assert(!(buffer.size() % 7)); // Should contain full information of each stroke
  auto total = buffer.size() / 7;

  std::vector<ColoredStroke> strokes;
  strokes.resize(total);

  for (size_t i = 0; i < total; ++i) {
    size_t offset = 7 * i;

    reinterpret_cast<Stroke&>(strokes[i]) = Stroke{
            {buffer[offset], buffer[offset + 1]}, {buffer[offset + 2], buffer[offset + 3]},
            {buffer[offset + 4], buffer[offset + 5]}, buffer[offset + 6]
    };
  }

  return strokes;
}

inline void colorize_strokes(std::vector<ColoredStroke>& strokes, const Image& image) {
  for (auto& stroke : strokes) find_stroke_color(stroke, image);
};

inline void colorize_strokes(std::vector<ColoredStroke>& strokes, const Color& setup_color) {
  for (auto& stroke : strokes) stroke.background_color = setup_color;
};

inline void colorize_strokes(std::vector<ColoredStroke>& strokes_to_colorize, const ImageStrokingData& data) {
  if (data.use_constant_color) colorize_strokes(strokes_to_colorize, data.stroke_color);
  else colorize_strokes(strokes_to_colorize, data.image);
};

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

    stroke_data_buffer[offset + 6] = strokes[i].width;
  }

  return stroke_data_buffer;
}

inline Image stroke_buffer_to_image(const std::vector<double>& stroke_buffer, const ImageStrokingData& data,
                                    Color canvas_color = {0., 0., 0.,}) {
  auto strokes = unpack_stroke_data_buffer(stroke_buffer);
  colorize_strokes(strokes, data);

  Image new_image = make_default_image(data.image.cols, data.image.rows, canvas_color);
  rasterize_strokes(new_image, strokes);

  return new_image;
}

inline void save_stroke_buffer_as_image(const std::vector<double>& stroke_buffer, const ImageStrokingData& data,
                                        const fs::path& filename, Color canvas_color) {
  Image image = stroke_buffer_to_image(stroke_buffer, data, canvas_color);
  save_image(image, filename);
}


inline void save_stroke_buffer_as_images(const std::vector<double>& stroke_buffer, const ImageStrokingData& data,
                                         const std::vector<fs::path>& filenames, Color canvas_color) {
  Image image = stroke_buffer_to_image(stroke_buffer, data, canvas_color);
  for (auto& filename : filenames) {
    save_image(image, filename);
  }
}


inline double transfer_range(double value, std::pair<double, double> from_range, std::pair<double, double> to_range) {
  double size_first = from_range.second - from_range.first;
  double size_second = to_range.second - to_range.first;

  return to_range.first + (size_second * (value - from_range.first) / size_first);
}

