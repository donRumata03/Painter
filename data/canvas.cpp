#include "canvas.h"


TransformImageData calc_transform_to_canvas(const Canvas& canvas, size_t w, size_t h) {
  Point vector(0, 0); // px
  bool album_canvas = canvas.width() >= canvas.height(); // Album orientation

  double scale = album_canvas ? (double) canvas.height(Units::MM) / h : (double) canvas.width(Units::MM) / w;
  vector.x = album_canvas ? (canvas.width(Units::MM) - scale * w) / 2 : 0;
  vector.y = album_canvas ? 0 : (canvas.height(Units::MM) - scale * h) / 2;

  return TransformImageData{vector, scale};
}


void to_json(json& j, const Canvas& canvas) {
  j["width"] = canvas.width(Units::MM);
  j["height"] = canvas.height(Units::MM);
  j["dpi"] = canvas.dpi();
}

void from_json(const json& j, Canvas& canvas) {
  assert(j.contains("width") && j["width"].is_number());
  assert(j.contains("height") && j["height"].is_number());
  assert(j.contains("dpi") && j["dpi"].is_number());

  canvas = Canvas(j["width"], j["height"], j["dpi"]);
}

ColoredStroke transform_stroke_into(const ColoredStroke &target, const Canvas &canvas, Units units_to) {
  auto res = target;

  auto transform = (units_to == Units::MM) ?
          std::function<double(double)>([&canvas](double px){ return canvas.px2mm(px); }) :
          std::function<double(double)>([&canvas](double mm){ return canvas.mm2px(mm); });

  auto transform_point = [&transform](Point& p){
    p.x = transform(p.x);
    p.y = transform(p.y);
  };

   transform_point(res.p0);
   transform_point(res.p1);
   transform_point(res.p2);

  return res;
}
