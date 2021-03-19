//
// Created by Vova on 19.03.2021.
//

#pragma once

#include "painter_pch.h"


inline fs::path glue_photos_into_video(const fs::path& folder_path, size_t w, size_t h, fs::path output_filename = "") {
	if (output_filename.empty()) output_filename = folder_path / "collage.mp4";

	if (fs::exists(output_filename)) fs::remove(output_filename);

	std::cout << console_colors::green << "Gluing images from " << folder_path << " into video " << output_filename << " by ffmpeg. " <<
		"If you experience an error, install FFMPEG, please!" << console_colors::remove_all_colors << std::endl;

	std::string moving_command = "cd \"" + folder_path.string() + "\"";
	std::cout << "Executing command: " << moving_command << std::endl;


	std::string ffmpeg_command =
			"ffmpeg -r 60 -f image2 "s
            // + "-s " + std::to_string(w) + "x" + std::to_string(h) + " "
			+ "-i %d.png " + output_filename.filename().string();

	std::cout << "Executing command: " << ffmpeg_command << std::endl;

	system((moving_command + " && " + ffmpeg_command).c_str());

	return output_filename;
}
