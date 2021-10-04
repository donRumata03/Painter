#pragma once


inline constexpr double kMmPerInch = 25.4;

inline constexpr size_t kDefaultCanvasWidth = 300;
inline constexpr size_t kDefaultCanvasHeight = 400;
inline constexpr size_t kDefaultCanvasDpi = 96;

enum class Units {
  PX = 0,
  MM
};
