#pragma once
#include "ILogger.hpp"
#include <memory>
#include <spdlog/async.h>
#include <spdlog/pattern_formatter.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

namespace MEngine::Core
{
class Spdlogger final : public ILogger
{
  private:
    void Log(LogLevel level, const std::source_location &loc, const std::string &message) override;
    spdlog::sink_ptr mConsoleSink;
    spdlog::sink_ptr mFileSink;
    std::shared_ptr<spdlog::logger> mLogger;

  public:
    Spdlogger();
    void SetLogLevel(LogLevel level) override;
    void Flush() override;
};
} // namespace MEngine::Core
