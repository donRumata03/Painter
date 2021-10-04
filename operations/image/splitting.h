#pragma once

#include <painter_pch.h>

#include <ostream>
#include <utility>

struct ImageCell {
  friend std::ostream& operator<<(std::ostream& os, const ImageCell& cell);

  ImageCell(size_t bottom, size_t upper, size_t left, size_t right);

  size_t min_y;
  size_t max_y;

  size_t min_x;
  size_t max_x;
};

class ImageZoneRepresentation {
 public:
  ImageZoneRepresentation() = delete;

  explicit ImageZoneRepresentation(std::vector<std::vector<ImageCell>> cells, size_t _image_w, size_t _image_h);

  [[nodiscard]] const std::vector<std::vector<ImageCell>>& get_2d_cells() const;
  const std::vector<ImageCell>& get_1d_cells() const;

  size_t get_image_w() const;
  size_t get_image_h() const;

  size_t get_x_zones() const;
  size_t get_y_zones() const;

  friend std::ostream& operator<<(std::ostream& os, const ImageZoneRepresentation& representation);

 private:
  size_t image_w, image_h;

  std::vector<std::vector<ImageCell>> cells_2d;
  mutable std::optional<std::vector<ImageCell>> cells_1d;
};

ImageZoneRepresentation schedule_image_splitting(size_t image_w, size_t image_h, size_t x_cells, 
  size_t y_cells, double relative_cell_overlay_size);
