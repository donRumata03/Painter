//
// Created by Elementarno on 17.02.2021.
//

#include "SVG_service.h"

#include "io_api/image_io_utils.h"

#define PATH_BOX_GOMOTHETY 1.1

SVG_service::SVG_service(const std::string& filepath, bool is_logging, const std::string &logging_path)
    : svg(lunasvg::SVGDocument()), is_logging(is_logging), logging_path(logging_path), it(0) {
    svg.loadFromFile(filepath);
    borders = get_shape_bounds(get_raster_image(svg));

    if (fs::exists(logging_path)) fs::remove_all(logging_path);
    fs::create_directories(logging_path);

    if (is_logging) std::cout << "[SVG_service] Created" << std::endl;
}

void SVG_service::split_paths() {
    auto root = svg.rootElement();

    // Split paths & render
    lunasvg::SVGElement *path = NULL;
    size_t count = 0;
    while ((path = root->getElementByTag("path", count)) != NULL) {
        lunasvg::SVGDocument path_doc;
        path_doc.rootElement()->setAttribute("viewBox", root->getAttribute("viewBox"));
        path_doc.appendElement(path);

        // Setup borders
        auto full_img = get_raster_image(path_doc);
        auto box = get_shape_bounds(full_img);
        box = limit_bounds(gomothety_bounds(box, PATH_BOX_GOMOTHETY), borders);
        path_doc.rootElement()->setAttribute("viewBox", to_viewbox(box));
        boxes.emplace_back(box);

        cv::imwrite(get_shape_path(count), get_raster_image(path_doc));
        count++;
    }

    shapes_count = count;

    if (is_logging) std::cout << "[SVG_service] Splited image into " << shapes_count << " parts." << std::endl;
}

bool SVG_service::load_current_image(cv::Mat &img) {
    auto path = get_shape_path(it);
    if (it >= shapes_count || !fs::exists(path)) {
        return false;
    }

    img = open_image(path);
    return true;
}

void SVG_service::shift_strokes(std::vector<colored_stroke>& strokes) {
    point vec(boxes[it].x, boxes[it].y);
    for (auto& stroke : strokes) {
        stroke.p0 = stroke.p0 + vec;
        stroke.p1 = stroke.p1 + vec;
        stroke.p2 = stroke.p2 + vec;
    }
}

cv::Mat SVG_service::get_raster_image(const lunasvg::SVGDocument& doc) {
    auto bitmap = doc.renderToBitmap();
    size_t width = bitmap.width(), height = bitmap.height();
    auto data = bitmap.data(); // array of bytes (RGBA)

    cv::Mat img = cv::Mat::zeros(height, width, CV_8UC3);
    for (size_t p = 0; p < width*height; p++) { // Need to convert RGBA -> BGR
        img.at<cv::Vec3b>(p / width, p % width) = {data[4*p + 2], data[4*p + 1], data[4*p]};
    }

    return img;
}

std::string SVG_service::get_shape_path(size_t i) {
    std::stringstream ss;
    ss << logging_path << "/" << "path" << i << ".png";
    return ss.str();
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