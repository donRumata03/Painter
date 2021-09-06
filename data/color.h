#pragma once

#include "painter_pch.h"


class BgrVec {}; // A tag type
class RgbVec {}; // A tag type


/**
 * Color class with rgb color model.
 * r, g, b are from 0 to 1 if T is floating Point type
 *
 * For T == uint8_t there are 256 * 256 * 256 = 16'777'216 possible values
 */
template <class T>
struct RgbColor {
  using DataType = T;

  T r{}, g{}, b{};

  RgbColor() = default;
  RgbColor(T _r, T _g, T _b) : r(_r), g(_g), b(_b) {}
  RgbColor(BgrVec, const cv::Vec<T, 3>& cv_bgr_color) : r(cv_bgr_color[2]), g(cv_bgr_color[1]), b(cv_bgr_color[0]) {}
  RgbColor(RgbVec, const cv::Vec<T, 3>& cv_rgb_color) : r(cv_rgb_color[0]), g(cv_rgb_color[1]), b(cv_rgb_color[2]) {}

  [[noreturn]] explicit RgbColor(const std::vector<T>& vector) {
    assert(vector.size() == 3);
    r = vector[0];
    g = vector[1];
    b = vector[2];
  }

  explicit RgbColor(const std::array<T, 3>& array) {
    r = array[0];
    g = array[1];
    b = array[2];
  }

  explicit RgbColor(const cv::Point3_<T>& cv_point);

  cv::Vec<T, 3> to_OpenCV_Vec3() const;
  [[nodiscard]] cv::Scalar to_OpenCV_Scalar() const;
  std::vector<T> to_vector() const;
  std::array<T, 3> to_array() const;

  T& operator[](size_t color_index);
  const T& operator[](size_t index) const;

  template <class Type>
  friend std::ostream& operator<<(std::ostream& os, const RgbColor<Type>& rgb_color);

  auto operator<=>(const RgbColor<T>&) const = default;

  double brightness();
};


/// RgbColor constructors and conversions to other color representations
template <class T>
std::vector<T> RgbColor<T>::to_vector() const {
  return std::vector<T>(r, g, b);
}

template <class T>
std::array<T, 3> RgbColor<T>::to_array() const {
  return std::array<T, 3>(r, g, b);
}

template <class T>
RgbColor<T>::RgbColor(const cv::Point3_<T>& cv_point) : r(cv_point.x), g(cv_point.y), b(cv_point.z) {}


/// RgbColor float convertors
template <class F, class I>
RgbColor<std::enable_if_t<std::is_floating_point_v<F>, F>> to_floating_point(
        const RgbColor<std::enable_if_t<std::is_integral_v<I>, I>>& initial_color);

template <class I, class F>
RgbColor<std::enable_if_t<std::is_integral_v<I>, I>> from_floating_point(
        const RgbColor<std::enable_if_t<std::is_floating_point_v<F>, F>>& initial_color);

template <class E, class T>
auto convert_color(const RgbColor<T>& initial_color) {
  static_assert(std::is_floating_point_v<T> ^ std::is_floating_point_v<E>,
                "Exactly one of the types should be floating Point!");
  if constexpr(std::is_floating_point_v<T>) {
    return from_floating_point<E, T>(initial_color);
  } else {
    return to_floating_point<E, T>(initial_color);
  }
}


template <class F, class I>
RgbColor<std::enable_if_t<std::is_floating_point_v<F>, F>> to_floating_point(
        const RgbColor<std::enable_if_t<std::is_integral_v<I>, I>>& initial_color) {
  F div_factor = 1 / F(std::numeric_limits<I>::max() - std::numeric_limits<I>::min());
  F offset = std::numeric_limits<I>::min();
  RgbColor<F> res = {
          div_factor * (F(initial_color.r) - offset),
          div_factor * (F(initial_color.g) - offset),
          div_factor * (F(initial_color.b) - offset)
  };
  return res;
}

template <class I, class F>
RgbColor<std::enable_if_t<std::is_integral_v<I>, I>> from_floating_point(
        const RgbColor<std::enable_if_t<std::is_floating_point_v<F>, F>>& initial_color) {
  auto range_checker = [](F value) { return F(0) <= value && F(1) >= value; };
  assert(range_checker(initial_color.r));
  assert(range_checker(initial_color.g));
  assert(range_checker(initial_color.b));

  F mul_factor = F(std::numeric_limits<I>::max() - std::numeric_limits<I>::min());
  F offset = std::numeric_limits<I>::min();
  RgbColor<I> res = {
          I(std::round(mul_factor * F(initial_color.r) + offset)),
          I(std::round(mul_factor * F(initial_color.g) + offset)),
          I(std::round(mul_factor * F(initial_color.b) + offset))
  };
  return res;
}


/// RGB_color operators
template <class T>
inline void hash_combine(std::size_t& seed, const T& v) {
  std::hash<T> hasher;
  seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

// TODO: make it converting operator
template <class T>
cv::Vec<T, 3> RgbColor<T>::to_OpenCV_Vec3() const {
  return cv::Vec<T, 3>{r, g, b};
}

template <class T>
cv::Scalar RgbColor<T>::to_OpenCV_Scalar() const {
  return cv::Scalar{r, g, b, 0.};
}


template <class T>
std::ostream& operator<<(std::ostream& os, const RgbColor<T>& rgb_color) {
  os << "RGB_Color { r: " << double(rgb_color.r) << " g: " << double(rgb_color.g) << " b: " << double(rgb_color.b)
     << " }";
  return os;
}

template <class T>
T& RgbColor<T>::operator[](size_t index) {
  assert(index <= 2);

  if (index == 0) return r;
  if (index == 1) return g;

  return b;
}

template <class T>
const T& RgbColor<T>::operator[](size_t index) const {
  return const_cast<const T&>(const_cast<RgbColor<T> *>(this)->operator[](index));
}

template <class T>
double RgbColor<T>::brightness() {
  return r * 0.3 + g * 0.59 + b * 0.11;
}

// To be able to put it into hash tables
namespace std {
  template <typename T>
  struct hash<RgbColor<T>> {
    inline size_t operator()(const RgbColor<T>& col) const {
      size_t seed = 0;
      ::hash_combine(seed, col.r);
      ::hash_combine(seed, col.g);
      ::hash_combine(seed, col.b);
      return seed;
    }
  };
}


/**
 * Color class with rgb color model and alpha channel.
 * r, g, b, a are all from 0 to 1 for T = double
 */
template <class T>
struct RgbaColor {
  T r, g, b, a;
};

/// Typedefs

using Pixel = cv::Point3_<double>;
using BytePixel = cv::Point3_<uint8_t>;

using ByteColor = RgbColor<uint8_t>;
using Color = RgbColor<double>;


template <class ColorType>
struct ColorTraits {
};

template <>
struct ColorTraits<ByteColor> {
  static constexpr uint8_t min = 0;
  static constexpr uint8_t max = 255;
  static constexpr uint8_t middle = 127;
};

template <>
struct ColorTraits<Color> {
  static constexpr double min = 0.;
  static constexpr double max = 1.;
  static constexpr double middle = 0.5;
};

/**
 * @param hex Representation of color in hexadecimal code
 * @return A color identical to hex code
 */
Color get_color_from_hex(const std::string& hex);

void to_json(json& j, const ByteColor& col);
void from_json(const json& j, ByteColor& col);
