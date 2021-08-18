#include "painter_pch.h"

// STD headers
#include <iostream>

// Libraries
#include <argparse/argparse.hpp>

// Project headers
#include "utils/Logger.h"
#include "launch/launch.h"

int main(int argc, char* argv[])
{
    set_utf8_in_console();

    Logger::SetLogFile(painter_base_path / "log" / "logs.txt");
    Logger::SetLoggingLevel(LogLevel::Info);

    argparse::ArgumentParser program("Painter");

    program.add_argument("path")
        .required();

    try {
        program.parse_args(argc, argv);
    } catch (const std::runtime_error& err) {
        ConsoleError("Main") << err.what();
        Console() << program;

        Logger::Stop();
        exit(0);
    }

    Console() << console_colors::green << "Welcome to Painter™!" << console_colors::remove_all_colors;
    LogConsoleInfo("Main") << "Using up to " << std::thread::hardware_concurrency() - 1 << " threads of " << std::thread::hardware_concurrency();

    std::string imagePath = program.get("path");

    auto image_path = program.get("path");

    if (!std::filesystem::exists(image_path)) {
        LogConsoleError("Main") << "Invalid image path: " << image_path;

        Logger::Stop();
        exit(0);
    }

    LaunchStroking(image_path);

    Logger::Stop();
    return 0;
}