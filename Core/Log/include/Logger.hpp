#pragma once
#include "ILogger.hpp"
#include <memory>

namespace MEngine::Core
{

class Logger final
{
  private:
    Logger();
    std::unique_ptr<ILogger> mLogger = nullptr;

  public:
    static Logger &GetInstance();
    ILogger *GetLogger() const
    {
        return mLogger.get();
    }
};

#define LogTrace(fmt, ...)                                                                                             \
    MEngine::Core::Logger::GetInstance().GetLogger()->Trace(std::source_location::current(), fmt, ##__VA_ARGS__)
#define LogDebug(fmt, ...)                                                                                             \
    MEngine::Core::Logger::GetInstance().GetLogger()->Debug(std::source_location::current(), fmt, ##__VA_ARGS__)
#define LogWarn(fmt, ...)                                                                                              \
    MEngine::Core::Logger::GetInstance().GetLogger()->Warn(std::source_location::current(), fmt, ##__VA_ARGS__)
#define LogInfo(fmt, ...)                                                                                              \
    MEngine::Core::Logger::GetInstance().GetLogger()->Info(std::source_location::current(), fmt, ##__VA_ARGS__)
#define LogError(fmt, ...)                                                                                             \
    MEngine::Core::Logger::GetInstance().GetLogger()->Error(std::source_location::current(), fmt, ##__VA_ARGS__)
#define LogFatal(fmt, ...)                                                                                             \
    MEngine::Core::Logger::GetInstance().GetLogger()->Fatal(std::source_location::current(), fmt, ##__VA_ARGS__)
} // namespace MEngine::Core
