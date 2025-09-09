#pragma once
#include <format>
#include <source_location>
#include <string>

namespace MEngine::Core
{

enum class LogLevel
{
    Trace,
    Debug,
    Info,
    Warn,
    Error,
    Fatal
};
class ILogger
{
  public:
    virtual ~ILogger() = default;
    virtual void Log(LogLevel level, const std::source_location &loc, const std::string &message) = 0;
    virtual void SetLogLevel(LogLevel level) = 0;
    virtual void Flush() = 0;

    template <typename... Args>
    void Trace(const std::source_location &loc, std::format_string<Args...> format, Args &&...args)
    {
        std::string message = std::format(format, std::forward<Args>(args)...);
        Log(LogLevel::Trace, loc, message);
    }
    template <typename... Args>
    void Debug(const std::source_location &loc, std::format_string<Args...> format, Args &&...args)
    {
        std::string message = std::format(format, std::forward<Args>(args)...);
        Log(LogLevel::Debug, loc, message);
    }
    template <typename... Args>
    void Warn(const std::source_location &loc, std::format_string<Args...> format, Args &&...args)
    {
        std::string message = std::format(format, std::forward<Args>(args)...);
        Log(LogLevel::Warn, loc, message);
    }
    template <typename... Args>
    void Info(const std::source_location &loc, std::format_string<Args...> format, Args &&...args)
    {
        std::string message = std::format(format, std::forward<Args>(args)...);
        Log(LogLevel::Info, loc, message);
    }
    template <typename... Args>
    void Error(const std::source_location &loc, std::format_string<Args...> format, Args &&...args)
    {
        std::string message = std::format(format, std::forward<Args>(args)...);
        Log(LogLevel::Error, loc, message);
    }
    template <typename... Args>
    void Fatal(const std::source_location &loc, std::format_string<Args...> format, Args &&...args)
    {
        std::string message = std::format(format, std::forward<Args>(args)...);
        Log(LogLevel::Fatal, loc, message);
    }
};
} // namespace MEngine::Core
