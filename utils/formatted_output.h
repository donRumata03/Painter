//
// Created by Vova on 23.07.2021.
//

#pragma once

#include <painter_pch.h>

inline std::string get_current_thread_info(size_t thread_index = 0)
{
	std::stringstream ss;
	ss << "Thread " << thread_index << " (" << std::this_thread::get_id() << ")";
	return ss.str();
}

inline std::string get_range_string(std::pair<size_t, size_t> job_range)
{
	std::stringstream ss;
	if (job_range.first < job_range.second) ss << "[" << job_range.first << ", " << job_range.second - 1 << "]";
	else ss << "ø";
	return ss.str();
}

