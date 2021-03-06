//
// Created by Elementarno on 17.02.2021.
//

#ifndef PAINTER_SVG_SERVICE_H
#define PAINTER_SVG_SERVICE_H

#include <svgdocument.h>

#include "painter_pch.h"
#include "data_representation/stroke.h"

class SVG_service {
public:
    explicit SVG_service(const std::string& filepath, bool is_logging = true,
                const std::string& logging_path = (fs::path{ painter_base_path } / "log" / "latest" / "svg").string());

    void split_paths();
    bool load_current_image(cv::Mat& img);

	template<class StrokeType>
    void shift_strokes_to_current_box(std::vector<StrokeType>& strokes);

    cv::Mat get_raster_original_image() { return get_raster_image(svg); }
    [[nodiscard]] cv::Rect get_borders() const { return borders; }
    [[nodiscard]] color get_current_color() const { return colors[it]; }

    [[nodiscard]] size_t get_it() const { return it; }
    [[nodiscard]] size_t get_shape_count() const { return shapes_count; }

    void set_iterator(size_t desired_iterator_value) { it = desired_iterator_value; }
    void next() { ++it; }
    void previous() { --it; }
    void restart() { it = 0; }

private:
    cv::Mat get_raster_image(const lunasvg::SVGDocument& doc);
    std::string get_shape_path(size_t i);

    cv::Rect get_shape_bounds(const cv::Mat& img);
    cv::Rect gomothety_bounds(const cv::Rect& bounds, double coeff);
    cv::Rect limit_bounds(const cv::Rect& original, const cv::Rect& limit);
    std::string to_viewbox(const cv::Rect& rect);
    cv::Rect from_viewbox(const lunasvg::SVGDocument& doc);
    cv::Rect from_viewbox(const std::string& vbox);
    color get_element_color(const lunasvg::SVGElement* elem);

    const static std::regex color_regex;

    lunasvg::SVGDocument svg;
    std::vector<cv::Rect> boxes;
    std::vector<color> colors;
    cv::Rect borders;

    bool is_logging;
    const std::string logging_path;
    size_t it = 0;
    size_t shapes_count = 0;
};


template<class StrokeType>
void SVG_service::shift_strokes_to_current_box(std::vector<StrokeType>& strokes) {
	point shifting_vector(boxes[it].x, boxes[it].y);
	for (auto& stroke : strokes) {
		shift_stroke(stroke, shifting_vector);
	}
}

#endif //PAINTER_SVG_SERVICE_H
