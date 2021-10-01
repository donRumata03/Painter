#pragma once

#include <svgdocument.h>

#include "painter_pch.h"
#include "data/stroke.h"
#include "data/canvas.h"
#include "data/vector_region.h"
#include "data/units.h"


class SvgService {
 public:
  SvgService() = default;

  explicit SvgService(const fs::path& filepath, const Canvas& canvas = Canvas::get_default_canvas(),
                      double critical_region_size = 10, bool is_logging = true,
                      const fs::path& logging_path = fs::path(painter_base_path) / "log" / "latest" / "svg");

  void split_paths(double gomothety_factor = 1.05, bool verbose = true);

  bool load_image(cv::Mat& img, size_t index) const;

  template <class StrokeType>
  void shift_strokes_to_box(std::vector<StrokeType>& strokes, size_t index);

  template <class StrokeType>
  void shift_strokes_to_canvas(std::vector<StrokeType>& strokes);

  template <class StrokeType>
  void transform_strokes_into_mm(std::vector<StrokeType>& strokes);

  template <class StrokeType>
  void transform_strokes_into_px(std::vector<StrokeType>& strokes);

  [[nodiscard]] cv::Mat get_raster_original_image() const;

  [[nodiscard]] cv::Rect get_borders() const { return svg_borders; }

  [[nodiscard]] size_t get_regions_count() const { return regions.size(); }

  [[nodiscard]] cv::Size get_workspace_size() const { return workspace_size; }

 private:
  [[nodiscard]] std::string get_region_path(size_t i) const;

  lunasvg::SVGDocument svg;
  std::vector<VectorRegion> regions;

  cv::Rect svg_borders;
  cv::Rect canvas_borders;
  cv::Size workspace_size;

  std::optional<Canvas> canvas;
  std::optional<TransformImageData> transform;


  bool is_logging;
  fs::path logging_path;
  double critical_region_size = 0;
};


template<class StrokeType>
void SvgService::shift_strokes_to_box(std::vector<StrokeType>& strokes, size_t i) {
  point shifting_vector(regions[i].box.x, regions[i].box.y);
  for (auto& stroke : strokes) {
    shift_stroke(stroke, shifting_vector);
  }
}

template<class StrokeType>
void SvgService::shift_strokes_to_canvas(std::vector<StrokeType>& strokes) {
  for (auto& stroke : strokes) {
    shift_stroke(stroke, transform->move_vector);
  }
}

template<class StrokeType>
void SvgService::transform_strokes_into_mm(std::vector<StrokeType>& strokes) {
  for (auto& stroke : strokes) {
    scale_stroke(stroke, 1. / canvas->dpi() * kMmPerInch);
  }
}

template<class StrokeType>
void SvgService::transform_strokes_into_px(std::vector<StrokeType>& strokes) {
  for (auto& stroke : strokes) {
    scale_stroke(stroke, 1. / kMmPerInch * canvas->dpi());
  }
}