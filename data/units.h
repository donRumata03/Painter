#pragma once


constexpr double kMmPerInch = 25.4;

constexpr size_t kDefaultCanvasWidth = 300;
constexpr size_t kDefaultCanvasHeight = 400;
constexpr size_t kDefaultCanvasDpi = 96;

enum class Units {
  PX = 0,
  MM
};
