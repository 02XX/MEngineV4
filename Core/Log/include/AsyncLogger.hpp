#pragma once
#include "ILogger.hpp"
#include <fstream>
#include <memory>
#include <moodycamel./concurrentqueue.h>
#include <thread>

namespace MEngine::Core
{
class AsyncLogger final : public ILogger
{
  private:
    moodycamel::ConcurrentQueue<std::pair<LogLevel, std::string>> mLogQueue;
    std::stop_source mStopSource;
    std::jthread mWorkerThread;
    std::ofstream mLogFile;
    LogLevel mLogLevel = LogLevel::Trace;

  private:
    void Log(LogLevel level, const std::source_location &loc, const std::string &message) override;

  public:
    AsyncLogger();
    ~AsyncLogger() override;
    void SetLogLevel(LogLevel level) override;
    void Flush() override;
    void WorkerThread(std::stop_token st);
};
} // namespace MEngine::Core
