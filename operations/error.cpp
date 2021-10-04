#include <execution>

#include "error.h"


double image_mse(const Image& image1, const Image& image2, bool parallel) {
  assert(image1.size == image2.size);
  size_t w = image1.cols, h = image1.rows;
  auto area = double(w * h);

  double diff_sum = 0;

  if (not parallel) {
    for (size_t x = 0; x < w; ++x) {
      for (size_t y = 0; y < h; ++y) {
        const auto& pixel1 = image1.at<cv::Vec3d>(y, x);
        const auto& pixel2 = image2.at<cv::Vec3d>(y, x);

        for (size_t dim_index = 0; dim_index < 3; ++dim_index) {
          diff_sum += square(pixel1[dim_index] - pixel2[dim_index]);
        }
      }
    }
  } else { // Parallel
    std::atomic<double> atomic_diff_sum = 0;

    image1.forEach<Pixel>([&](Pixel& first_pixel_value, const int position[]) {
      double this_diff_sum = 0;

      const auto first_pixel = Color(first_pixel_value);
      const auto second_pixel = Color(image2.at<cv::Vec3d>(position[0], position[1]));

      // TODO: divide pixels into areas to improve performance!

      for (size_t dim_index = 0; dim_index < 3; ++dim_index) {
        this_diff_sum += square(second_pixel[dim_index] - first_pixel[dim_index]);
      }

      atomic_diff_sum += this_diff_sum;
    });

    diff_sum = atomic_diff_sum;
  }

  return diff_sum / area;
}

double stroke_mse(const Image& image, const ColoredStroke& stroke, size_t step_number, bool parallel) {
  return stroke_mse(image, stroke.get_points(step_number, get_image_range_limits(image)), stroke.background_color,
                    parallel);
}

double stroke_mse(const Image& image, const std::vector<Point>& stroke_points, const Color& stroke_color,
                  bool parallel) {
  auto stroke_color_vec = stroke_color.to_OpenCV_Vec3();

  double total_diff_sum = 0;
  size_t points_in_stroke = 0;

  auto point_handler = [&](const Point& point_xy) {
    size_t x = point_xy.x;
    size_t y = point_xy.y;
    const auto& image_pixel_color = image.at<cv::Vec3d>(y, x);

    for (size_t dim_index = 0; dim_index < 3; ++dim_index) {
      total_diff_sum += square(stroke_color[dim_index] - image_pixel_color[dim_index]);
    }

    points_in_stroke++;

  };

  if (parallel) {
    std::for_each(std::execution::par_unseq, stroke_points.begin(), stroke_points.end(), point_handler);
  } else {
    std::for_each(std::execution::seq, stroke_points.begin(), stroke_points.end(), point_handler);
  }

  return total_diff_sum / points_in_stroke;
}