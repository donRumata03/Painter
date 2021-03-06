#include "paint_plan.h"

PaintPlan::PaintPlan(std::vector<colored_stroke> strokes)
    : strokes(std::move(strokes)), palette(get_palette<byte_color>(this->strokes)) { }


template<class Color>
std::set<Color> get_palette(const std::vector<colored_stroke>& strokes)
{
    static_assert(std::is_base_of_v<rgb_color<typename Color::Type>, Color>, "Pallete is only used for colors!");

    std::set<Color> pallete;
    for (auto& s : strokes) {
        pallete.insert(convert_color<typename Color::Type>(s.background_color));
    }
    return pallete;
}

void to_json(json& j, const PaintPlan& plan)
{
    std::map<byte_color, size_t> color_idx;
    auto it = plan.palette.begin();
    for (size_t i = 0; it != plan.palette.end(); i++, it++) {
        color_idx[*it] = i;
    }

    for (size_t i = 0; i < plan.strokes.size(); i++) {
        j["strokes"][i] = (stroke&)plan.strokes[i];
        j["strokes"][i]["color_id"] = color_idx[convert_color<uint8_t>(plan.strokes[i].background_color)];
    }

    j["colors"] = plan.palette;
}

void from_json(const json& j, PaintPlan& plan)
{
    std::vector<byte_color> pallete = j["colors"];

    for (const auto &data : j["strokes"])
    {
        assert(data.contains("color_id") && data["color_id"].is_number());
        assert(0 <= data["color_id"] && data["color_id"] < pallete.size());

        colored_stroke col_stroke;
        from_json(data, (stroke&)col_stroke);
        col_stroke.background_color = convert_color<double>(pallete[data["color_id"]]);
        plan.strokes.emplace_back(col_stroke);
    }

    plan.palette = std::set<byte_color>(pallete.begin(), pallete.end());

}