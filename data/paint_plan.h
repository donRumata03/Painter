#pragma once

#include "painter_pch.h"
#include "data/stroke.h"
#include "data/canvas.h"


struct PaintPlan {
  PaintPlan() = default;
  explicit PaintPlan(std::vector<ColoredStroke> strokes, Canvas canvas);

  auto operator<=>(const PaintPlan& paint_plan) const = default;

  std::vector<ColoredStroke> strokes;
  std::unordered_set<ByteColor> palette;
  Canvas canvas;
};

template <class Color>
std::unordered_set<Color> get_palette(const std::vector<ColoredStroke>& strokes);

void to_json(json& j, const PaintPlan& plan);
void from_json(const json& j, PaintPlan& plan);


