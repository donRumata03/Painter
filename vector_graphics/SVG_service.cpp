//
// Created by Elementarno on 17.02.2021.
//

#include <svgelementiter.h>

#include "SVG_service.h"
#include "io_api/image_io_utils.h"
#include <common_operations/basic_constraining.h>
#include <common_operations/filesystem_primitives.h>
#include "utils/Progress.h"

#define PATH_BOX_GOMOTHETY 1.05
#define CRITICAL_WIDTH 2
#define CRITICAL_HEIGHT 2

const std::regex SVG_service::color_regex = std::regex("fill:#([a-fA-F0-9]{6})");
const std::regex SVG_service::path_regex = std::regex("(<path[\\w\\W]+?>)");

static size_t count_substrings(const std::string& source, const std::string& sub)
{
    int count = 0;
    for (size_t offset = source.find(sub); offset != std::string::npos;
         offset = source.find(sub, offset + sub.length()))
    {
        count++;
    }
    return count;
}

static cv::Rect2i scale_rect(const cv::Rect2i& rect, double scale_factor)
{
    return cv::Rect2i(scale_factor * rect.x,
                      scale_factor * rect.y,
                      scale_factor * rect.width,
                      scale_factor * rect.height);
}

SVG_service::SVG_service(const fs::path& filepath, const Canvas& canvas, bool is_logging, const fs::path& logging_path)
    : svg(lunasvg::SVGDocument()), canvas(canvas), is_logging(is_logging), logging_path(logging_path), it(0) {

    svg.loadFromFile(filepath.string());
    borders = get_shape_bounds(get_raster_image(svg));
    shapes_count = count_substrings(svg.toString(), "<path");

    transform.emplace(calc_transform_to_canvas(canvas, borders.width, borders.height));

	ensure_log_cleared(logging_path);

    if (is_logging) std::cout << "[SVG_service] Created" << std::endl;
}

void SVG_service::split_paths() {
    auto root = svg.rootElement();

    // Split paths & render
    size_t count = 0;
    lunasvg::SVGElementIter iter(root, "", "path");
    Progress progress(shapes_count);
    while (iter.next()) {
        lunasvg::SVGDocument path_doc;
        path_doc.rootElement()->setAttribute("viewBox", root->getAttribute("viewBox"));
        path_doc.appendElement(iter.currentElement());

        // Setup borders
        auto box = get_shape_bounds(get_raster_image(path_doc));
        if (box.width <= CRITICAL_WIDTH || box.height <= CRITICAL_HEIGHT) {
            progress.update();
            continue; // TODO: find a better solution
        }


        box = limit_bounds(gomothety_bounds(box, PATH_BOX_GOMOTHETY), borders);
        path_doc.rootElement()->setAttribute("viewBox", to_viewbox(box));
        boxes.emplace_back(box);
        colors.emplace_back(get_element_color(iter.currentElement()));

        // std::cout << "[SVG_service] Saving raster image of part #" << count << " to " << get_shape_path(count) << "…" << std::endl;
        cv::imwrite(get_shape_path(count),
                    get_raster_image(path_doc, transform->scale_factor * box.width, transform->scale_factor * box.height));

        count++;
        progress.update();
    }

    shapes_count = count;

    if (is_logging) std::cout << "[SVG_service] Splited image into " << shapes_count << " parts." << std::endl;
}

bool SVG_service::load_current_image(cv::Mat &img) const {
    auto path = get_shape_path(it);
    if (it >= shapes_count || !fs::exists(path)) {
        return false;
    }

    img = open_image(path);
    return true;
}



cv::Mat SVG_service::get_raster_image(const lunasvg::SVGDocument& doc, size_t width, size_t height) const {
    auto bitmap = doc.renderToBitmap(width, height);
    width = bitmap.width();
    height = bitmap.height();
    auto data = bitmap.data(); // array of bytes (RGBA)

    cv::Mat img = cv::Mat::zeros(height, width, CV_8UC3);
    for (size_t p = 0; p < width*height; p++) { // Need to convert RGBA -> BGR
        img.at<cv::Vec3b>(p / width, p % width) = {data[4*p + 2], data[4*p + 1], data[4*p]};
    }

    return img;
}

std::string SVG_service::get_shape_path(size_t i) const {
	return (fs::path{ logging_path } / ("path" + std::to_string(i) + ".png")).string();
//    std::stringstream ss;
//    ss << logging_path << "/" << "path" << i << ".png";
//    return ss.str();
}

cv::Rect SVG_service::get_shape_bounds(const cv::Mat &img) {
    Image gray;
    cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
    return cv::boundingRect(gray);
}

std::string SVG_service::to_viewbox(const cv::Rect &rect) {
    std::stringstream ss;
    ss << rect.x << " " << rect.y << " " << rect.width << " " << rect.height;
    return ss.str();
}

cv::Rect SVG_service::from_viewbox(const std::string &vbox) {
    auto pos = split(vbox);
    return cv::Rect(std::stod(pos[0]),std::stod(pos[1]),
                    std::stod(pos[2]), std::stod(pos[3]));
}

cv::Rect SVG_service::from_viewbox(const lunasvg::SVGDocument &doc) {
    return from_viewbox(doc.rootElement()->getAttribute("viewBox"));
}

color SVG_service::get_element_color(const lunasvg::SVGElement* elem)
{
    std::smatch match;
    std::string property = elem->getAttribute("style");
    if (std::regex_search(property, match, color_regex)) {
        return get_color_from_hex(match[1]);
    } else throw std::runtime_error("Invalid color tried to parse");
}

cv::Rect SVG_service::gomothety_bounds(const cv::Rect &bounds, double coeff) {
    assert(coeff > 0);
    return cv::Rect(bounds.x - (coeff-1)/2 * bounds.width,
                    bounds.y - (coeff-1)/2 * bounds.height,
                    coeff * bounds.width,
                    coeff * bounds.height);
}

cv::Rect SVG_service::limit_bounds(const cv::Rect& original, const cv::Rect& limit) {
    int new_x = std::max(original.x, limit.x), new_y = std::max(original.y, limit.y);
    return cv::Rect(new_x,
                    new_y,
                    std::min(original.x + original.width, limit.x + limit.width) - new_x,
                    std::min(original.y + original.height, limit.y + limit.height) - new_y);
}
