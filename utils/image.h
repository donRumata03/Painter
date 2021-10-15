#pragma once

#include "data/color.h"

struct ColoredPosition {
  size_t x = 0;
  size_t y = 0;
  Color c{};
};

/**
 * Create a template image in RGB (double, 3 channels)
 */
Image make_default_image(size_t w, size_t h, const Color& color = {});

/**
 * Create a template image in RGB (double, 3 channels)
 */
Image make_default_image(size_t w, size_t h, double grey_brightness);

/**
 * Create a template image in RGB (double, 3 channels)
 */
Image make_default_image(const cv::Size& size, const Color& color = {});

/**
 * Create a template image in RGB (double, 3 channels)
 */
Image make_default_image(const cv::Size& size, double grey_brightness);

/**
 * Open image in RGB (double, 3 channels)
 */
Image open_image(const std::string& filename);

/**
 * Show image by openCV
 * @attention Assume image is in RGB, elements are of type double
 */
void cv_show_image(const Image& img, const std::string& image_name = "Image");

/**
 * Show image by system viewer
 * @attention Assume image is in RGB, elements are of type double
 */
void show_image_in_system_viewer(const Image& img, const std::string& temp_name = "");

/**
 * Save image
 * @attention Assume image is in RGB, elements are of type double
 */
void save_image(const Image& img, const fs::path& filename);

/**
 * Convert colors in image between RGB and BGR
 * @attention It works vice versa: RGB to BGR and BGR to RGB
 */
template <class PixelType>
void rgb2bgr(Image& img) {
  img.forEach<PixelType>([](PixelType& pixel, const int position[]) {
    std::swap(pixel.x, pixel.z);
  });
}

/**
 * Convert colors in image between RGB and BGR
 * @attention It works vice versa: RGB to BGR and BGR to RGB
 */
template <class PixelType>
Image rgb2bgr(const Image& img) {
  auto new_image = img.clone();
  rgb2bgr<PixelType>(new_image);
  return new_image;
}

/**
 * Convert colors in image between BGR to RGB
 * @attention It works vice versa: RGB to BGR and BGR to RGB
 */
template <class PixelType>
void bgr2rgb(Image& img) {
  rgb2bgr<PixelType>(img);
}

/**
 * Convert colors in image between BGR to RGB
 * @attention It works vice versa: RGB to BGR and BGR to RGB
 */
template <class PixelType>
Image bgr2rgb(const Image& img) {
  return rgb2bgr<PixelType>(img);
}

/**
 * Convert colors in image from RGB to HSV
 */
Image rgb2hsv(const Image& img);

/**
 * Convert colors in image from HSV to RGB
 */
Image hsv2rgb(const Image& img);

/**
 * Convert colors in image from CV_8UC3 to CV_64FC3
 */
Image convert_image_to_floating_point(const Image& img);

/**
 * Convert colors in image from CV_64FC3 to CV_8UC3
 */
Image convert_image_from_floating_point(const Image& img);

/**
 * Fill the image with one color
 */
void fill_image(Image& image, const Color& fill_color, bool parallelize = false);
