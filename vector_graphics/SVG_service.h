//
// Created by Elementarno on 17.02.2021.
//

#ifndef PAINTER_SVG_SERVICE_H
#define PAINTER_SVG_SERVICE_H

#include <svgdocument.h>

#include "painter_pch.h"
#include "data_representation/stroke.h"
#include "data_representation/canvas.h"

class SVG_service {
public:
    SVG_service() = default;
    explicit SVG_service(const fs::path& filepath, const Canvas& canvas = Canvas::get_default_canvas(), bool is_logging = true,
                const fs::path& logging_path = fs::path{ painter_base_path } / "log" / "latest" / "svg");

    void split_paths();
    bool load_current_image(cv::Mat& img) const;

	template<class StrokeType>
    void shift_strokes_to_current_box(std::vector<StrokeType>& strokes);

    template<class StrokeType>
    void shift_strokes_to_canvas(std::vector<StrokeType>& strokes);

    template<class StrokeType>
    void transform_strokes_into_mm(std::vector<StrokeType>& strokes);

    cv::Mat get_raster_original_image() { return get_raster_image(svg, transform->scale_factor * borders.width, transform->scale_factor * borders.height); }
    [[nodiscard]] cv::Rect get_borders() const { return borders; }
    [[nodiscard]] color get_current_color() const { return colors[it]; }

    [[nodiscard]] size_t get_it() const { return it; }
    [[nodiscard]] size_t get_shapes_count() const { return shapes_count; }

    void set_iterator(size_t desired_iterator_value) const { it = desired_iterator_value; }
    void next() const { ++it; }
    void previous() const { --it; }
    void restart() const { it = 0; }

private:
    cv::Mat get_raster_image(const lunasvg::SVGDocument& doc, size_t width = 0, size_t height = 0) const;
    std::string get_shape_path(size_t i) const;

    cv::Rect get_shape_bounds(const cv::Mat& img);
    cv::Rect gomothety_bounds(const cv::Rect& bounds, double coeff);
    cv::Rect limit_bounds(const cv::Rect& original, const cv::Rect& limit);
    std::string to_viewbox(const cv::Rect& rect);
    cv::Rect from_viewbox(const lunasvg::SVGDocument& doc);
    cv::Rect from_viewbox(const std::string& vbox);
    color get_element_color(const lunasvg::SVGElement* elem);

    const static std::regex color_regex;
    const static std::regex path_regex;

private:
    lunasvg::SVGDocument svg;
    std::vector<cv::Rect> boxes;
    std::vector<color> colors;
    cv::Rect borders;

    std::optional<Canvas> canvas;
    std::optional<TransformImageData> transform;

    bool is_logging;
    fs::path logging_path;
    mutable size_t it = 0;
    size_t shapes_count = 0;
};


template<class StrokeType>
void SVG_service::shift_strokes_to_current_box(std::vector<StrokeType>& strokes) {
	point shifting_vector(transform->scale_factor * boxes[it].x, transform->scale_factor * boxes[it].y);
	for (auto& stroke : strokes) {
		shift_stroke(stroke, shifting_vector);
	}
}

template<class StrokeType>
void SVG_service::shift_strokes_to_canvas(std::vector<StrokeType>& strokes) {
    for (auto& stroke : strokes) {
        shift_stroke(stroke, transform->move_vector);
    }
}

template<class StrokeType>
void SVG_service::transform_strokes_into_mm(std::vector<StrokeType>& strokes) {
    for (auto& stroke : strokes) {
        scale_stroke(stroke, 1. / canvas->dpi() * MM_PER_INCH);
    }
}

#endif //PAINTER_SVG_SERVICE_H
