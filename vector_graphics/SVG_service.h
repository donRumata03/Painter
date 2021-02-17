//
// Created by Elementarno on 17.02.2021.
//

#ifndef PAINTER_SVG_SERVICE_H
#define PAINTER_SVG_SERVICE_H

#include "painter_pch.h"

#include "svgdocument.h"

class SVG_service {
public:
    SVG_service(const std::string& filepath, bool is_logging = true,
                const std::string& logging_path = (fs::path{ painter_base_path} / "log" / "svg").string());

    void split_paths();
    bool load_next_image(cv::Mat& img);
private:
    cv::Mat get_rastr_image(const lunasvg::SVGDocument& doc);
    std::string get_shape_path(size_t i);

    cv::Rect get_shape_bounds(const cv::Mat& img);
    cv::Rect gomothety_bounds(const cv::Rect& bounds, double coeff);
    cv::Rect limit_bounds(const cv::Rect& original, const cv::Rect& limit);
    std::string to_viewbox(const cv::Rect& rect);
    cv::Rect from_viewbox(const lunasvg::SVGDocument& doc);
    cv::Rect from_viewbox(const std::string& vbox);


    lunasvg::SVGDocument svg;
    std::vector<cv::Rect> boxes;
    cv::Rect borders;

    bool is_logging;
    const std::string logging_path;
    size_t it = 0;
    size_t shapes_count = 0;
};


#endif //PAINTER_SVG_SERVICE_H
