#include "utils.h"

// TODO: realize tanh, functions
double smooth_transmit_between_borders_linearal(double max_dimensions) {
  return max_dimensions;
}

double smooth_transmit_between_borders_tanh(double const max_dimensions, double max_width) {
  double arg_offset = 3.5;
  double exp = std::pow(e, 2 * (max_dimensions - arg_offset));
  return  max_width * exp / (exp + 1);
}

double smooth_transmit_between_borders_sigmoid(double const max_dimensions, double max_width) {
  double arg_offset = 3.5;
  double arg = -(max_dimensions - arg_offset);
  double exp = std::pow(e, arg);
  return max_width * (1.0 / (1.0 + exp));
}

double fatness_to_width(const Stroke& stroke, double fatness) {
  auto bounding_box = stroke.get_curve_bounding_box();
  double dimension_x = bounding_box.max_x - bounding_box.min_x;
  double dimension_y = bounding_box.max_y - bounding_box.min_y;
  double max_dimensions = std::max(dimension_x, dimension_y);

  double max_width = 10;

  return smooth_transmit_between_borders_sigmoid(max_dimensions, max_width) * fatness;
}

std::vector<ColoredStroke> unpack_stroke_data_buffer(const std::vector<double>& buffer) {
  assert(!(buffer.size() % 7)); // Should contain full information of each stroke
  auto total = buffer.size() / 7;

  std::vector<ColoredStroke> strokes;
  strokes.resize(total);

  for (size_t i = 0; i < total; ++i) {
    size_t offset = 7 * i;

    reinterpret_cast<Stroke&>(strokes[i]) = Stroke{
            {buffer[offset], buffer[offset + 1]}, {buffer[offset + 2], buffer[offset + 3]},
            {buffer[offset + 4], buffer[offset + 5]}, 1 //buffer [offset + 6]
    };

    strokes[i].width = fatness_to_width(strokes[i], buffer[offset + 6]);
  }

  return strokes;
}

void colorize_strokes(std::vector<ColoredStroke>& strokes, const Image& image) {
  for (auto& stroke : strokes) find_stroke_color(stroke, image);
};

void colorize_strokes(std::vector<ColoredStroke>& strokes, const Color& setup_color) {
  for (auto& stroke : strokes) stroke.background_color = setup_color;
};

void colorize_strokes(std::vector<ColoredStroke>& strokes_to_colorize, const ImageStrokingData& data) {
  if (data.use_constant_color) colorize_strokes(strokes_to_colorize, data.stroke_color);
  else colorize_strokes(strokes_to_colorize, data.image);
};

double width_to_fatness(const Stroke& stroke, double width) {
  auto bounding_box = stroke.get_curve_bounding_box();
  double dimension_x = bounding_box.max_x - bounding_box.min_x;
  double dimension_y = bounding_box.max_y - bounding_box.min_y;
  double max_dimensions = std::max(dimension_x, dimension_y);

  return width / smooth_transmit_between_borders_sigmoid(max_dimensions, 10);
}

Image stroke_buffer_to_image(const std::vector<double>& stroke_buffer, const ImageStrokingData& data,
                                    Color canvas_color) {
  auto strokes = unpack_stroke_data_buffer(stroke_buffer);
  colorize_strokes(strokes, data);

  Image new_image = make_default_image(data.image.cols, data.image.rows, canvas_color);
  rasterize_strokes(new_image, strokes);

  return new_image;
}

void save_stroke_buffer_as_image(const std::vector<double>& stroke_buffer, const ImageStrokingData& data,
                                        const fs::path& filename, Color canvas_color) {
  Image image = stroke_buffer_to_image(stroke_buffer, data, canvas_color);
  save_image(image, filename);
}


void save_stroke_buffer_as_images(const std::vector<double>& stroke_buffer, const ImageStrokingData& data,
                                         const std::vector<fs::path>& filenames, Color canvas_color) {
  Image image = stroke_buffer_to_image(stroke_buffer, data, canvas_color);
  for (auto& filename : filenames) {
    save_image(image, filename);
  }
}


double transfer_range(double value, std::pair<double, double> from_range, std::pair<double, double> to_range) {
  double size_first = from_range.second - from_range.first;
  double size_second = to_range.second - to_range.first;

  return to_range.first + (size_second * (value - from_range.first) / size_first);
}

Packer::Packer(const CommonStrokingParams& params) { this->params = params; }

std::vector<ColoredStroke> Packer::unpack_stroke_data_buffer(const std::vector<double>& buffer) {
  assert(!(buffer.size() % 7));  // Should contain full information of each stroke
  auto total = buffer.size() / 7;

  std::vector<ColoredStroke> strokes;
  strokes.resize(total);

  for (size_t i = 0; i < total; ++i) {
    size_t offset = 7 * i;

    reinterpret_cast<Stroke&>(strokes[i]) = Stroke{
        {buffer[offset], buffer[offset + 1]},
        {buffer[offset + 2], buffer[offset + 3]},
        {buffer[offset + 4], buffer[offset + 5]},
        1  // buffer [offset + 6]
    };

    strokes[i].width = fatness_to_width(strokes[i], buffer[offset + 6]);
  }

  return strokes;
}

double Packer::fatness_to_width(const Stroke& stroke, double fatness) {
  auto bounding_box = stroke.get_curve_bounding_box();
  double dimension_x = bounding_box.max_x - bounding_box.min_x;
  double dimension_y = bounding_box.max_y - bounding_box.min_y;
  double max_dimensions = std::max(dimension_x, dimension_y);

  double max_width = params.stroke_width.second;

  return smooth_transmit_between_borders_sigmoid(max_dimensions, max_width) *
         fatness;
}

double Packer::smooth_transmit_between_borders_tanh(double const max_dimensions,
                                            double max_width) {
  double arg_offset = 3.5;
  double exp = std::pow(e, 2 * (max_dimensions - arg_offset));
  return max_width * exp / (exp + 1);
}

double Packer::smooth_transmit_between_borders_sigmoid(double const max_dimensions,
                                               double max_width) {
  double arg_offset = 3.5;
  double arg = -(max_dimensions - arg_offset);
  double exp = std::pow(e, arg);
  return max_width * (1.0 / (1.0 + exp));
}

double Packer::width_to_fatness(const Stroke& stroke, double width) {
  auto bounding_box = stroke.get_curve_bounding_box();
  double dimension_x = bounding_box.max_x - bounding_box.min_x;
  double dimension_y = bounding_box.max_y - bounding_box.min_y;
  double max_dimensions = std::max(dimension_x, dimension_y);

  double max_width = params.stroke_width.second;
  return width / smooth_transmit_between_borders_sigmoid(max_dimensions, max_width);
}