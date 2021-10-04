#include "utils.h"


size_t count_substrings(const std::string& source, const std::string& sub) {
  int count = 0;
  for (size_t offset = source.find(sub); offset != std::string::npos;
       offset = source.find(sub, offset + sub.length())) {
    count++;
  }
  return count;
}

cv::Rect2i scale_rect(const cv::Rect2i& rect, double scale_factor) {
  return cv::Rect2i(scale_factor * rect.x, scale_factor * rect.y,
                    scale_factor * rect.width, scale_factor * rect.height);
}

cv::Mat get_raster_image(const lunasvg::SVGDocument& doc, size_t width, size_t height) {
  auto bitmap = doc.renderToBitmap(width, height);
  width = bitmap.width();
  height = bitmap.height();
  auto data = bitmap.data(); // Array of bytes (RGBA)

  cv::Mat img = cv::Mat::zeros(height, width, CV_8UC3);
  for (size_t p = 0; p < width * height; p++) { // Need to convert RGBA -> BGR
    img.at<cv::Vec3b>(p / width, p % width) = {data[4 * p + 2], data[4 * p + 1], data[4 * p]};
  }

  return img;
}

cv::Rect get_region_bounds(const cv::Mat& img) {
  Image gray;
  cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
  return cv::boundingRect(gray);
}

std::string to_viewbox(const cv::Rect& rect) {
  std::stringstream ss;
  ss << rect.x << " " << rect.y << " " << rect.width << " " << rect.height;
  return ss.str();
}

cv::Rect from_viewbox(const std::string& vbox) {
  auto pos = split(vbox);
  return cv::Rect(std::stod(pos[0]), std::stod(pos[1]),
                  std::stod(pos[2]), std::stod(pos[3]));
}

cv::Rect from_viewbox(const lunasvg::SVGDocument& doc) {
  return from_viewbox(doc.rootElement()->getAttribute("viewBox"));
}

Color get_element_color(const lunasvg::SVGElement *elem) {
  const static std::regex color_regex = std::regex("fill:#([a-fA-F0-9]{6})");

  std::smatch match;
  std::string property = elem->getAttribute("style");
  if (std::regex_search(property, match, color_regex)) {
    return get_color_from_hex(match[1]);
  } else throw std::runtime_error("Invalid color tried to parse");
}

cv::Rect gomothety_bounds(const cv::Rect& bounds, double coeff, double min_padding) {
  assert(coeff > 0);
  double dx = (coeff - 1) / 2 * bounds.width, dy = (coeff - 1) / 2 * bounds.height;
  if (dx < min_padding || dy < min_padding) {
    dx = min_padding;
    dy = min_padding;
  }
  return cv::Rect(bounds.x - min_padding, bounds.y - min_padding,
                  bounds.width + 2 * min_padding, bounds.height + 2 * min_padding);
}

cv::Rect limit_bounds(const cv::Rect& original, const cv::Rect& limit) {
  int new_x = std::max(original.x, limit.x), new_y = std::max(original.y, limit.y);
  return cv::Rect(new_x,
                  new_y,
                  std::min(original.x + original.width, limit.x + limit.width) - new_x,
                  std::min(original.y + original.height, limit.y + limit.height) - new_y);
}
