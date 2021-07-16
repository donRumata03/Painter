//
// Created by Elementarno on 12.03.2021.
//

#include "canvas.h"

Canvas::Canvas(size_t mm_width, size_t mm_height, size_t dpi)
    : mm_width(mm_width), mm_height(mm_height), _dpi(dpi) {

}

TransformImageData calc_transform_to_canvas(const Canvas &canvas, size_t w, size_t h) {
    //assert(canvas.width > w);
    //assert(canvas.height > h);

    point vector(0, 0); // px
    bool album_canvas = canvas.width() >= canvas.height(); // Album orientation

    double scale = album_canvas ? (double)canvas.height(Units::MM) / h : (double)canvas.width(Units::MM) / w;
    vector.x = album_canvas ? (canvas.width(Units::MM) - scale * w) / 2 : 0;
    vector.y = album_canvas ? 0 : (canvas.height(Units::MM) - scale * h) / 2;

    return TransformImageData { vector, scale };
}


void to_json(json& j, const Canvas& canvas)
{
    j["width"] = canvas.width(Units::MM);
    j["height"] = canvas.height(Units::MM);
    j["dpi"] = canvas.dpi();
}

void from_json(const json& j, Canvas& canvas)
{
    assert(j.contains("width") && j["width"].is_number());
    assert(j.contains("height") && j["height"].is_number());
    assert(j.contains("dpi") && j["dpi"].is_number());

    canvas = Canvas(j["width"], j["height"], j["dpi"]);
}