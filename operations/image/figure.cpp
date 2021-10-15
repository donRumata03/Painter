#include "figure.h"

size_t painted_pixel_number_black_canvas(const Image& image) {
  Image gray, thresh, u_img = convert_image_from_floating_point(image);
  cv::cvtColor(u_img, gray, cv::COLOR_BGR2GRAY);

  cv::threshold(gray, thresh, 0, 255, cv::THRESH_BINARY);

  cv::imwrite((painter_base_path / "log" / "grey.png").string(), gray);
  cv::imwrite((painter_base_path / "log" / "thresh.png").string(), thresh);

  size_t pixels = cv::sum(thresh)[0] / 255;

  return pixels;
}

size_t painted_pixel_number(const Image& image, Color canvas_color) {
  std::atomic<size_t> painted_pixels = 0;

  image.forEach<Pixel>([&painted_pixels, &canvas_color /*, &m*/](Pixel& pixel, const int position[]) {
    auto this_pixel_color = Color{pixel};

    if (this_pixel_color != canvas_color) {
      painted_pixels++;
    }
  });

  return painted_pixels;
}

std::vector<ColoredPosition> get_painted_pixels(const Image& image, Color canvas_color) {
  std::vector<ColoredPosition> pixels;

  for (size_t y = 0; y < image.rows; y++) {
    for (size_t x = 0; x < image.cols; x++) {
      auto color = Color(image.at<cv::Vec3d>(y,x));
      if (color != canvas_color) {
        pixels.emplace_back(ColoredPosition{x, y, color});
      }
    }
  }

  return pixels;
}

size_t perimeter_length_pixels(const Image& image) {
  // TODO!
  return 0;
}
