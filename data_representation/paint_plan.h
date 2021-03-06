#ifndef PAINTER_PAINT_PLAN_H
#define PAINTER_PAINT_PLAN_H

#include <utility>

#include "painter_pch.h"
#include "stroke.h"
#include "color.h"

struct PaintPlan {
    PaintPlan() = default;
    explicit PaintPlan(std::vector<colored_stroke> strokes);

    std::vector<colored_stroke> strokes;
    std::unordered_set<byte_color> palette;
};

template<class Color>
std::set<Color> get_palette(const std::vector<colored_stroke>& strokes);

void to_json(json& j, const PaintPlan& plan);
void from_json(const json& j, PaintPlan& plan);

#endif //PAINTER_PAINT_PLAN_H