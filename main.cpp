#include "painter_pch.h"

// STD headers
#include <iostream>

// Libraries
#include <argparse/argparse.hpp>

// Project headers
#include "utils/logger.h"
#include "launch/launch.h"


int main(int argc, char *argv[]) {
  set_utf8_in_console();

  Logger::SetLogFile(std::filesystem::current_path() / "log" / "logs.txt");
  Logger::SetLoggingLevel(LogLevel::Info);

  argparse::ArgumentParser program("Painter");

  program.add_argument("path")
          .required()
          .help("Path to target image");

  program.add_argument("--params")
          .help("Path to common stroking params");

  try {
    program.parse_args(argc, argv);
  } catch (const std::runtime_error& err) {
    ConsoleError("Main") << err.what();
    Console() << program;

    Logger::Stop();
    exit(0);
  }

  Console() << console_colors::green << "Welcome to Painter™!" << console_colors::remove_all_colors;
  LogConsoleInfo("Main") << "Using up to " << std::thread::hardware_concurrency() - 1 << " threads of "
                         << std::thread::hardware_concurrency();

  auto image_path = program.get("path");

  try {
    if (!std::filesystem::exists(image_path)) {
      throw std::runtime_error("Invalid image path: " + image_path);
    }
    LogConsoleInfo("Main") << "Image path: " << image_path;

    CommonStrokingParams params;
    auto params_path = program.get("--params");
    if (!params_path.empty()) {
      if (!std::filesystem::exists(params_path)) {
        throw std::runtime_error("Invalid params path: " + params_path);
      }
      params = load_params(params_path);
      LogConsoleSuccess("Main") << "Params loaded from: " << params_path;
    }

    launch_stroking(image_path, params);
  } catch (const std::exception& ex) {
    LogConsoleError("Exception") << ex.what();
    Logger::Stop();
    exit(1);
  }

  Logger::Stop();
  return 0;
}