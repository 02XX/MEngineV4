#include "Logger.hpp"
#include "AsyncLogger.hpp"
#include "Spdlogger.hpp"
namespace MEngine::Core
{
Logger &Logger::GetInstance()
{
    static Logger instance;
    return instance;
}
Logger::Logger()
{
    mLogger = std::make_unique<Spdlogger>();
}
} // namespace MEngine::Core