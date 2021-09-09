#pragma once

// Libraries

// - Pythonic
#include <pythonic.h>

// - PowerfulGA
#include <GA/GA_optimizer.h>

// - OpenCV
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>

// Typedefs
using Image = cv::Mat;
using Point = point;
using Limit = std::pair<double, double>;

// Constants
#include "utils_constexpr/stringing.h"
inline constexpr const char* painter_pch_file_path = __FILE__;
inline std::filesystem::path painter_base_path = get_path_folder(painter_pch_file_path);
