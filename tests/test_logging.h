#pragma once

#include "utils/Logger.h"

inline void test_logging()
{
    Logger::SetLogFile(painter_base_path / "log" / "test_log.txt");
    Logger::SetLoggingLevel(LogLevel::Debug);

    ConsoleDebug("Main") << "Console";
    LogDebug("Main") << "Log";
    LogConsoleDebug("Main") << "LogConsole";

    ConsoleInfo("Main") << "Console";
    LogInfo("Main") << "Log";
    LogConsoleInfo("Main") << "LogConsole";

    Console() << "Console";
    Log() << "Log";
    LogConsole() << "LogConsole";

    ConsoleSuccess("Main") << "Console";
    LogSuccess("Main") << "Log";
    LogConsoleSuccess("Main") << "LogConsole";

    ConsoleWarning("Main") << "Console";
    LogWarning("Main") << "Log";
    LogConsoleWarning("Main") << "LogConsole";

    ConsoleError("Main") << "Console";
    LogError("Main") << "Log";
    LogConsoleError("Main") << "LogConsole";

    Logger::SetLoggingLevel(LogLevel::Common);
    LogConsoleDebug() << "Test detail";
    LogConsoleInfo() << "Test detail";
    LogConsole() << "Test detail";
    LogConsoleSuccess() << "Test detail";
    LogConsoleWarning() << "Test detail";
    LogConsoleError() << "Test detail";
    Logger::SetLoggingLevel(LogLevel::Debug);

    LogConsole() << "";

    LogConsoleInfo("Module", "Submodule", "Subsubmodule") << "Text";
    LogConsoleError("File", "Function", "Line", "Byte", "Atom", "Electron") << "Just stop...";
}
