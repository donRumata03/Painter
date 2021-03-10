#pragma once

#include <chrono>
#include <string>
#include <sstream>
#include <iostream>
#include <math.h>

/// Possible signs:
// ⇛, 🢖, 🡆, ⇝, ⟿, ↝, ☛, ☞, ⟼, ➽, ❱, ⇉, ➠

// static const std::vector<std::string> bars = { "▯", "▉", "❱" };
static const char* bars[] = { " ", "=", ">" };

static double round(double value, uint8_t presicion) {
    uint64_t multiplicator = std::pow(10, presicion);
    return int(value * multiplicator) / (double)multiplicator;
}

class Progress {
private:
    size_t total, current, start;
    uint8_t totalWidth;
    std::chrono::time_point<std::chrono::system_clock> t_start = std::chrono::system_clock::now();
    std::chrono::time_point<std::chrono::system_clock> t_current;
    bool is_working = true;

    uint8_t _total_str_width;

    double get_progress() { return current / (double)total; }
    double get_time_per_it() { return round(std::chrono::duration<double>(t_current - t_start).count() / (current - start), 2); }

    static std::string bar_string(double progress, size_t size)
    {
        size -= 2; // Extra symbols
        double step = 1. / size;
        std::stringstream ss;
        ss << "[";
        for (size_t i = 0; i < size; i++) {
            if (i == (int)(progress * size)) ss << bars[2];
            else if (i < progress * size) ss << bars[1];
            else ss << bars[0];
        }
        ss << "]";
        return ss.str();
    }

    std::string get_info() {
        std::stringstream ss;
        ss.width(_total_str_width);
        ss << current << " / " << total;
        if (current - start) ss << " (" << get_time_per_it() << " s/it)";
        return ss.str();
    }
public:
    explicit Progress(size_t total, size_t start = 0, uint8_t totalWidth = 70)
            : total(total), start(start), current(start), totalWidth(totalWidth) {
        _total_str_width = std::to_string(total).size();
        draw();
    }

    void end() {
        is_working = false;
        std::cout << std::endl;
    }

    void update(size_t add = 1) {
        if (!is_working) return;

        current += add;
        t_current = std::chrono::system_clock::now();
        draw();
        if (current == total) end();
    }

    void draw() {
        if (!is_working) return;

        std::string info = get_info();
        uint8_t barWidth = totalWidth - info.size() - 1;
        std::cout << bar_string(get_progress(), barWidth) << " " << info << "\r";
        std::cout.flush();
    }
};