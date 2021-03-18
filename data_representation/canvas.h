//
// Created by Elementarno on 12.03.2021.
//

#ifndef PAINTER_CANVAS_H
#define PAINTER_CANVAS_H

#include "painter_pch.h"

inline constexpr double MM_PER_INCH = 25.4;

enum Units {
    PX = 0,
    MM
};

struct TransformImageData {
    point move_vector;
    double scale_factor;
};

class Canvas {
private:
    size_t mm_width;
    size_t mm_height;

    size_t _dpi;

public:
    Canvas(size_t mm_width, size_t mm_height, size_t dpi = 96);

    [[nodiscard]] size_t width(Units units = PX) const { return units == PX ? mm_width / MM_PER_INCH * _dpi : mm_width; }
    [[nodiscard]] size_t height(Units units = PX) const { return units == PX ? mm_height / MM_PER_INCH * _dpi : mm_height; }

	[[nodiscard]] size_t dpi() const { return _dpi; }
};

TransformImageData calc_transform_to_canvas(const Canvas& canvas, size_t w, size_t h);

#endif //PAINTER_CANVAS_H
