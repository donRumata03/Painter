#include "common_stroking_params.h"


CommonStrokingParams load_params(const std::string& path) {
  std::ifstream stream(path);
  json j;
  stream >> j;
  stream.close();
  return j.get<CommonStrokingParams>();
}

void to_json(json& j, const CommonStrokingParams& params) {
  j["stroke_number"] = params.stroke_number;
  j["units"] = params.units == Units::MM ? "mm" : "px";
  j["is_relative"] = params.is_relative;
  j["stroke_length"] = params.stroke_length;
  j["stroke_width"] = params.stroke_width;
  j["move_mutation_probability"] = params.move_mutation_probability;
  j["canvas_color"] = params.canvas_color;
  j["stroke_color"] = params.stroke_color;
  j["use_constant_color"] = params.use_constant_color;
  j["logging_percentage"] = params.logging_percentage;
}

void from_json(const json& j, CommonStrokingParams& params) {
  CommonStrokingParams new_params;

  if (j.contains("stroke_number")) {
    new_params.stroke_number = j["stroke_number"];
  }

  if (j.contains("units")) {
    assert(j["units"] == "mm" || j["units"] == "px");
    new_params.units = j["units"] == "mm" ? Units::MM : Units::PX;
  }

  if (j.contains("is_relative")) {
    assert(j["is_relative"].is_boolean());
    new_params.is_relative = j["is_relative"];
  }

  if (j.contains("stroke_length")) {
    new_params.stroke_length = j["stroke_length"];
  }

  if (j.contains("stroke_width")) {
    new_params.stroke_width = j["stroke_width"];
  }

  if (j.contains("move_mutation_probability")) {
    new_params.move_mutation_probability = j["move_mutation_probability"];
  }

  if (j.contains("canvas_color")) {
    new_params.canvas_color = j["canvas_color"];
  }

  if (j.contains("stroke_color")) {
    new_params.stroke_color = j["stroke_color"];
  }

  if (j.contains("use_constant_color")) {
    new_params.use_constant_color = j["use_constant_color"];
  }

  if (j.contains("logging_percentage")) {
    new_params.logging_percentage = j["logging_percentage"];
  }

  params = new_params;
}
