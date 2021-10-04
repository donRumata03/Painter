#pragma once

#include "painter_pch.h"
#include "data/common_stroking_params.h"

void launch_stroking(const std::string& filename, const CommonStrokingParams& params = CommonStrokingParams{},
                     const fs::path& logging_path = painter_base_path / "log" / "latest");