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
          .required()
          .help("Path to common stroking params");

  try {
    program.parse_args(argc, argv);
  } catch (const std::runtime_error& err) {
    ConsoleError("Main") << "Failed to parse command line arguments: " << err.what();
    Console() << program;

    Logger::Stop();
    exit(0);
  }

  Console() << console_colors::green << "Welcome to Painter™!" << console_colors::remove_all_colors;

  if (std::thread::hardware_concurrency() <= 1) {
  	LogConsoleWarning("Main") << "StrongNishebrodWarning: YOU HAVE ONLY ONE THREAD! Hopefully, you won't try multithreading mode…";
  }
  else if (std::thread::hardware_concurrency() <= 2) {
  	LogConsoleWarning("Main")
  	    << "NishebrodWarning: do you seriously have only " << std::thread::hardware_concurrency() << " threads in >= 2021?! "
  	    << "Buying a new computer strongly recommended.";
  }
  LogConsoleInfo("Main") << "Using up to " << std::thread::hardware_concurrency() - 1 << " threads of "
                         << std::thread::hardware_concurrency();

  auto image_path = program.get("path");

  try {
    if (not fs::exists(image_path)) {
      throw std::runtime_error("Image path doesn't exist: " + image_path);
    } else if (not fs::is_regular_file(image_path)) {
      throw std::runtime_error("Image path you specified doesn't correspond to regular file: " + image_path);
    }
    LogConsoleInfo("Main") << "Image path: " << image_path;

    CommonStrokingParams params;
    auto params_path = program.get("--params");

    if (not fs::exists(params_path)) {
      throw std::runtime_error("Parameters path you provided doesn't exist: " + params_path);
    } else if (fs::path(params_path).extension().string() != ".json") {
      throw std::runtime_error(
              "Parameters path has inappropriate extension: \"" + fs::path(params_path).extension().string() + R"(" (".json" expected))"
      );
    }
    params = load_params(params_path);
    LogConsoleSuccess("Main") << "Parameters loaded from: " << params_path;

    launch_stroking(image_path, params);
  } catch (const std::exception& ex) {
    LogConsoleError("Exception") << ex.what();
    Logger::Stop();
    exit(1);
  }

  Logger::Stop();
  return 0;
}