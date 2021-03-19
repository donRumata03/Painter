//
// Created by Vova on 19.03.2021.
//

#pragma once

#include "painter_pch.h"


inline fs::path glue_photos_into_video(const fs::path& folder_path, const fs::path& output_filename = "") {
	if (output_filename.empty()) output_filename = folder_path / "collage.mp4";

	// TODO:
	std::string command = "ffmpeg " + ;

	system(command);

	return output_filename
}
