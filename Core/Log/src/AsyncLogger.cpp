#include "AsyncLogger.hpp"
#include "ILogger.hpp"
#include <chrono>
#include <filesystem>
#include <format>
#include <iostream>
#include <syncstream>
using namespace std::chrono_literals;
namespace MEngine::Core
{

void AsyncLogger::Log(LogLevel level, const std::source_location &loc, const std::string &message)
{
    auto file = std::filesystem::path(loc.file_name()).filename().string();
    auto line = loc.line();
    auto function = loc.function_name();
    auto log = std::format("[{}:{}] {}", file, line, message);
    mLogQueue.enqueue({level, log});
}

AsyncLogger::AsyncLogger()
{
    std::filesystem::path logDir = std::filesystem::current_path() / "logs";
    std::filesystem::path logFile = logDir / "app.log";
    if (!std::filesystem::exists(logDir))
    {
        std::filesystem::create_directories(logDir);
    }
    mLogFile.open(std::filesystem::current_path() / "logs/app.log", std::ios::app);
    if (!mLogFile.is_open())
    {
        throw std::runtime_error("Failed to open log file");
    }
    mWorkerThread = std::jthread(&AsyncLogger::WorkerThread, this, mStopSource.get_token());
}
AsyncLogger::~AsyncLogger()
{
    mStopSource.request_stop();
    mWorkerThread.join();
    mLogFile.close();
}
void AsyncLogger::SetLogLevel(LogLevel level)
{
    mLogLevel = level;
}

void AsyncLogger::Flush()
{
}
void AsyncLogger::WorkerThread(std::stop_token st)
{
    std::pair<LogLevel, std::string> log;
    int color = 34; // Default to blue
    std::string level = "TRACE";
    while (!st.stop_requested() || mLogQueue.try_dequeue(log))
    {
        if (log.first < mLogLevel)
            continue;
        auto now = std::chrono::system_clock::now();
        auto time = std::format("{:%H:%M:%S}", now);
        switch (log.first)
        {
        case LogLevel::Trace: {
            color = 34;
            level = "TRACE";
            break;
        }
        case LogLevel::Debug: {
            color = 36;
            level = "DEBUG";
            break;
        }
        case LogLevel::Info: {
            color = 32;
            level = "INFO";
            break;
        }
        case LogLevel::Warn: {
            color = 33;
            level = "WARN";
            break;
        }
        case LogLevel::Error: {
            color = 31;
            level = "ERROR";
            break;
        }
        case LogLevel::Fatal: {
            color = 35;
            level = "FATAL";
            break;
        }
        }
        log.second = std::format("{} [{}] {}", time, level, log.second);
        std::osyncstream(std::cout) << std::format("\033[{}m{}\033[0m", color, log.second) << std::endl;
        mLogFile << log.second << std::endl;
    }
}

} // namespace MEngine::Core
