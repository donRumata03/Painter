#pragma once

#include <chrono>
#include <string>
#include <sstream>
#include <iostream>
#include <math.h>

/// Possible signs:
// ⇛, 🢖, 🡆, ⇝, ⟿, ↝, ☛, ☞, ⟼, ➽, ❱, ⇉, ➠

static const std::vector<std::string> bars = {" ", "▏","▎","▍","▌","▋","▊","▉","█"};
static const std::pair<std::string, std::string> borders = {"▏", "▕"};
static const double time_threshold = 0.5;
static const uint8_t time_presicion = 2;
// static const std::vector<std::string> bars = { " ", "▉", "❱" };
// static const char* bars[] = { " ", "=", ">" };

static double round(double value, uint8_t presicion) {
    uint64_t multiplicator = std::pow(10, presicion);
    return (int)(value * multiplicator) / (double)multiplicator;
}

class Progress {
private:
    size_t total, current, start;
    uint8_t barWidth;
    std::chrono::time_point<std::chrono::system_clock> t_start = std::chrono::system_clock::now();
    std::chrono::time_point<std::chrono::system_clock> t_current;
    bool is_working = true;

    uint8_t _total_str_width = 0;
    uint8_t max_width = 0;

    double get_progress() { return current / (double)total; }
    double get_time_per_it() { return std::chrono::duration<double>(t_current - t_start).count() / (current - start); }

    static std::string bar_string(double progress, size_t size)
    {
        size -= 2; // Extra symbols
        double length = progress * size, filled_length = 0;
        modf(length, &filled_length);
        uint8_t idx = std::floor((length - filled_length) * bars.size());
        std::stringstream ss;

        ss << borders.first;
        for (size_t i = 0; i < size; i++) {
            if (i < filled_length) ss << bars[bars.size() - 1];
            else if (i == filled_length) ss << bars[idx];
            else ss << bars[0];
        }
        ss << borders.second;

        return ss.str();
    }

    std::string get_info() {
        std::stringstream ss;
        ss.width(_total_str_width);
        ss << current << " / " << total;
        if (current - start) {
            double time_per_it = get_time_per_it();
            size_t time_passed = std::chrono::duration_cast<std::chrono::seconds>(t_current - t_start).count();
            size_t total_time_prediction = time_passed / get_progress();
            ss << " [";
            ss << std::setfill('0') << std::setw(2) << (time_passed / 60) << ":" << std::setw(2) << (time_passed % 60) << "<"
                << std::setw(2) << (total_time_prediction / 60) << ":" << std::setw(2) << (total_time_prediction % 60) << " ";

            if (time_per_it > time_threshold) ss << round(time_per_it, time_presicion) << " s/it";
            else ss << round(1. / time_per_it, time_presicion) << " it/s";
            ss << "]";
        }
        return ss.str();
    }
public:
    explicit Progress(size_t total, size_t start = 0, uint8_t barWidth = 30)
            : total(total), start(start), current(start), barWidth(barWidth) {
        _total_str_width = std::to_string(total).size();
        Draw();
    }

    void End() {
        is_working = false;
        std::cout << std::endl;
    }

    void Update(size_t add = 1) {
        if (!is_working) return;

        current += add;
        t_current = std::chrono::system_clock::now();
        Draw();
        if (current == total) End();
    }

    bool IsWorking() { return is_working; }

    void Clear()
    {
        std::cout << std::string(max_width, ' ') << "\r";
    }

    void Draw() {
        if (!is_working) return;

        std::stringstream ss;
        ss << bar_string(get_progress(), barWidth) << " " << get_info();
        std::string result = ss.str();
        max_width = std::max((uint8_t)result.size(), max_width);
        std::cout << result << "\r";
        std::cout.flush();
    }

    std::chrono::time_point<std::chrono::system_clock>& GetLastUpdateTime() { return t_current; }
};