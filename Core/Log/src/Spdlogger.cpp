#include "Spdlogger.hpp"
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <spdlog/async.h>

namespace MEngine::Core
{
class UppercaseLevelFormatter : public spdlog::custom_flag_formatter
{
  public:
    void format(const spdlog::details::log_msg &msg, const std::tm &, spdlog::memory_buf_t &dest) override
    {
        std::string level = spdlog::level::to_string_view(msg.level).data();
        std::transform(level.begin(), level.end(), level.begin(), tolower);
        level[0] = toupper(level[0]);
        dest.append(level.data(), level.data() + level.size());
    }

    std::unique_ptr<custom_flag_formatter> clone() const override
    {
        return std::make_unique<UppercaseLevelFormatter>();
    }
};
Spdlogger::Spdlogger()
{
    try
    {
        spdlog::init_thread_pool(8192, 1);

        mConsoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        mFileSink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
            (std::filesystem::current_path() / "Logs/app.log").string().c_str(), 5 * 1024 * 1024, 3);
        // 异步日志
        mLogger =
            std::make_shared<spdlog::async_logger>("MEngineLogger", spdlog::sinks_init_list{mConsoleSink, mFileSink},
                                                   spdlog::thread_pool(), spdlog::async_overflow_policy::block);
        // 同步日志
        // mLogger = std::make_shared<spdlog::logger>("MEngineLogger", spdlog::sinks_init_list{mConsoleSink,
        // mFileSink});

        auto formatter = std::make_unique<spdlog::pattern_formatter>();
        formatter->add_flag<UppercaseLevelFormatter>('U');
        formatter->set_pattern("%^[%Y-%m-%d %H:%M:%S.%e] [%20s:%-4#] %U: %v%$");
        mLogger->set_formatter(std::move(formatter));
    }
    catch (const spdlog::spdlog_ex &e)
    {
        std::cerr << "Logger initialization failed: " << e.what() << std::endl;
    }
}
void Spdlogger::SetLogLevel(LogLevel level)
{
    switch (level)
    {
    case LogLevel::Trace:
        mLogger->set_level(spdlog::level::trace);
        break;
    case LogLevel::Debug:
        mLogger->set_level(spdlog::level::debug);
        break;
    case LogLevel::Info:
        mLogger->set_level(spdlog::level::info);
        break;
    case LogLevel::Warn:
        mLogger->set_level(spdlog::level::warn);
        break;
    case LogLevel::Error:
        mLogger->set_level(spdlog::level::err);
        break;
    case LogLevel::Fatal:
        mLogger->set_level(spdlog::level::critical);
        break;
    }
}
void Spdlogger::Log(LogLevel level, const std::source_location &loc, const std::string &message)
{
    switch (level)
    {
    case LogLevel::Trace:
        mLogger->log(spdlog::source_loc{loc.file_name(), static_cast<int>(loc.line()), loc.function_name()},
                     spdlog::level::trace, message);
        break;
    case LogLevel::Debug:
        mLogger->log(spdlog::source_loc{loc.file_name(), static_cast<int>(loc.line()), loc.function_name()},
                     spdlog::level::debug, message);
        break;
    case LogLevel::Info:
        mLogger->log(spdlog::source_loc{loc.file_name(), static_cast<int>(loc.line()), loc.function_name()},
                     spdlog::level::info, message);
        break;
    case LogLevel::Warn:
        mLogger->log(spdlog::source_loc{loc.file_name(), static_cast<int>(loc.line()), loc.function_name()},
                     spdlog::level::warn, message);
        break;
    case LogLevel::Error:
        mLogger->log(spdlog::source_loc{loc.file_name(), static_cast<int>(loc.line()), loc.function_name()},
                     spdlog::level::err, message);
        break;
    case LogLevel::Fatal:
        mLogger->log(spdlog::source_loc{loc.file_name(), static_cast<int>(loc.line()), loc.function_name()},
                     spdlog::level::critical, message);
        break;
    }
}
void Spdlogger::Flush()
{
    mLogger->flush();
}
} // namespace MEngine::Core
