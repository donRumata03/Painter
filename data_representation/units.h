#ifndef PAINTER_UNITS_H
#define PAINTER_UNITS_H

inline constexpr double MM_PER_INCH = 25.4;

inline constexpr size_t default_canvas_width = 300;
inline constexpr size_t default_canvas_height = 400;
inline constexpr size_t default_canvas_dpi = 96;

enum class Units {
    PX = 0,
    MM
};

#endif
