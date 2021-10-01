#include <svgelementiter.h>

#include "svg_service.h"
#include "data/vector_region.h"
#include "vector_graphics/utils.h"
#include "operations/stroke/limits.h"
#include "utils/image.h"
#include "utils/progress.h"
#include "utils/logger.h"


SvgService::SvgService(const fs::path& filepath, const Canvas& canvas, double critical_region_size, bool is_logging,
                       const fs::path& logging_path)
        : svg(lunasvg::SVGDocument()), canvas(canvas), critical_region_size(critical_region_size), is_logging(is_logging),
          logging_path(logging_path) {

  svg.loadFromFile(filepath.string());
  svg_borders = from_viewbox(svg);

  transform.emplace(calc_transform_to_canvas(canvas, svg_borders.width, svg_borders.height));
  canvas_borders = scale_rect(svg_borders, transform->scale_factor);
  workspace_size = cv::Size(canvas.mm2px(canvas_borders.width), canvas.mm2px(canvas_borders.height));

  ensure_log_cleared(logging_path);

  LogConsoleSuccess("SVG Service") << "Inited";
}


void SvgService::split_paths(double gomothety_factor, bool verbose) {
  auto root = svg.rootElement();

  // Split paths & render
  size_t count = 0, total = count_substrings(svg.toString(), "<path");
  lunasvg::SVGElementIter iter(root, "", "path");
  Logger::NewProgress(total);
  while (iter.next()) {
    lunasvg::SVGDocument region_doc;
    region_doc.rootElement()->setAttribute("viewBox", root->getAttribute("viewBox"));
    region_doc.appendElement(iter.currentElement());

    // Setup borders
    auto svg_box = get_region_bounds(get_raster_image(region_doc));
    auto box = scale_rect(svg_box, transform->scale_factor);
    LogInfo("SVG Service") << "Region #" << Logger::GetCurrentProgress() << ": " << box.width << " mm, " << box.height
                           << " mm";
    if (box.width <= critical_region_size || box.height <= critical_region_size) {
      Logger::UpdateProgress();
      continue; // TODO: find a better solution
    }


    svg_box = limit_bounds(gomothety_bounds(svg_box, gomothety_factor, 5), svg_borders);
    box = scale_rect(limit_bounds(gomothety_bounds(box, gomothety_factor, 5), canvas_borders),
                     1. / kMmPerInch * canvas->dpi());
    region_doc.rootElement()->setAttribute("viewBox", to_viewbox(svg_box));

    VectorRegion region(get_raster_image(region_doc, box.width, box.height), box,
                        get_element_color(iter.currentElement()), get_region_path(count));
    regions.emplace_back(region);

    cv::imwrite(region.path, region.image);
    LogInfo("SVG Service") << "Saved raster image of path #" << count << " to " << region.path;

    count++;
    Logger::UpdateProgress();
  }

  LogConsoleSuccess("SVG Service") << "Splitted image into " << regions.size() << " parts";
}


bool SvgService::load_image(cv::Mat& img, size_t index) const {
  if (index < 0 || index >= regions.size()) {
    return false;
  }

  img = regions[index].image;

  return true;
}

cv::Mat SvgService::get_raster_original_image() const {
  return get_raster_image(svg, svg_borders.width, svg_borders.height);
}

std::string SvgService::get_region_path(size_t i) const {
  return (logging_path / ("region" + std::to_string(i) + ".png")).string();
}
