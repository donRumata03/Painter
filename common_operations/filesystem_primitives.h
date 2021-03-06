//
// Created by Vova on 06.03.2021.
//

#pragma once

#include <painter_pch.h>



inline void ensure_latest_log_cleared()
{
	auto latest_log_path = painter_base_path / "log" / "latest";

	if(fs::exists(latest_log_path)) fs::remove_all(latest_log_path);
	fs::create_directories(latest_log_path);
}

