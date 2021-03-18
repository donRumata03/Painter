//
// Created by Elementarno on 12.03.2021.
//

#include "canvas.h"

TransformImageData::TransformImageData(point move_vector, double scale_factor)
    : move_vector(move_vector), scale_factor(scale_factor) { }

Canvas::Canvas(size_t mm_width, size_t mm_height, size_t dpi)
    : mm_width(mm_width), mm_height(mm_height), _dpi(dpi) {

}

TransformImageData calc_transform_to_canvas(const Canvas &canvas, size_t w, size_t h) {
    //assert(canvas.width > w);
    //assert(canvas.height > h);

    double scale = 1.;
    point vector(0, 0); // px
    bool album_canvas = canvas.width() >= canvas.height(); // Album orientation

    scale = album_canvas ? (double)canvas.height() / h : (double)canvas.width() / w;
    vector.x = album_canvas ? (canvas.width() - scale * w) / 2 : 0;
    vector.y = album_canvas ? 0 : (canvas.height() - scale * h) / 2;

    return TransformImageData(vector, scale);
}

