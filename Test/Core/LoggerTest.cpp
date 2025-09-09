#include "Logger.hpp"
#include <gtest/gtest.h>

using namespace MEngine::Core;
TEST(LoggerTest, singleton)
{
    Logger &logger = Logger::GetInstance();
    EXPECT_NE(logger.GetLogger(), nullptr);
}
TEST(LoggerTest, LogTest)
{
    LogTrace("Test Trace, {}, {}, {}, {:.1f}", 1, "test", true, 3.14);
    LogDebug("Test Debug, {}, {}, {}, {:.1f}", 1, "test", true, 3.14);
    LogInfo("Test Info, {}, {}, {}, {:.1f}", 1, "test", true, 3.14);
    LogWarn("Test Warn, {}, {}, {}, {:.1f}", 1, "test", true, 3.14);
    LogError("Test Error, {}, {}, {}, {:.1f}", 1, "test", true, 3.14);
    LogFatal("Test Fatal, {}, {}, {}, {:.1f}", 1, "test", true, 3.14);
}
