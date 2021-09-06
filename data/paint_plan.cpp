#include "paint_plan.h"


PaintPlan::PaintPlan(std::vector<ColoredStroke> strokes, Canvas canvas)
        : strokes(std::move(strokes)), palette(get_palette<ByteColor>(this->strokes)), canvas(std::move(canvas)) {}


template <class Color>
std::unordered_set<Color> get_palette(const std::vector<ColoredStroke>& strokes) {
  static_assert(std::is_base_of_v<RgbColor<typename Color::DataType>, Color>, "Palette is only used for colors!");

  std::unordered_set<Color> palette;
  for (auto& s : strokes) {
    palette.insert(convert_color<typename Color::DataType>(s.background_color));
  }
  return palette;
}

void to_json(json& j, const PaintPlan& plan) {
  std::map<ByteColor, size_t> color_idx;
  auto it = plan.palette.begin();
  for (size_t i = 0; it != plan.palette.end(); i++, it++) {
    color_idx[*it] = i;
  }

  for (size_t i = 0; i < plan.strokes.size(); i++) {
    j["strokes"][i] = ContextWrapper<Stroke>{
            plan.strokes[i],
            plan.canvas
    };
    j["strokes"][i]["color_id"] = color_idx[convert_color<uint8_t>(plan.strokes[i].background_color)];
  }

  j["colors"] = plan.palette;
  j["canvas"] = plan.canvas;
}

void from_json(const json& j, PaintPlan& plan) {
  plan.canvas = j["canvas"];

  std::vector<ByteColor> pallete = j["colors"];

  for (const auto& data : j["strokes"]) {
    assert(data.contains("color_id") && data["color_id"].is_number());
    assert(0 <= data["color_id"] && data["color_id"] < pallete.size());

    ColoredStroke col_stroke;
    from_json(data, (Stroke&) col_stroke, plan.canvas);
    col_stroke.background_color = convert_color<double>(pallete[data["color_id"]]);
    plan.strokes.emplace_back(col_stroke);
  }

  plan.palette = std::unordered_set<ByteColor>(pallete.begin(), pallete.end());
}