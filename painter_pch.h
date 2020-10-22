//
// Created by Vova on 13.07.2020.
//

#pragma once

// My libraries:
#include <pythonic.h>
#include <GA/GA_optimizer.h>

// OpenCV:
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>

// My constants:

#include "utils_constexpr/stringing.h"
inline constexpr const char* painter_pch_file_path = __FILE__;
inline std::filesystem::path painter_base_path = get_path_folder(painter_pch_file_path);
