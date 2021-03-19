//
// Created by Vova on 06.03.2021.
//

#pragma once

#include <painter_pch.h>



inline void ensure_log_cleared(const fs::path& log_path = painter_base_path / "log" / "latest")
{
	if(fs::exists(log_path)) {
		// std::cout << "Clearing log path: " << log_path.string() << "…" << std::endl;
		fs::remove_all(log_path);
	}
	fs::create_directories(log_path);
}

