#include "log.h"

#include <spdlog/common.h>
#include <spdlog/logger.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <memory>
#include <string>

struct calculator::Logger::Impl
{
    std::shared_ptr<spdlog::logger> customLogger;
};

calculator::Logger::Logger() : impl_(std::make_unique<Impl>())
{
    try
    {
        impl_->customLogger =
            spdlog::basic_logger_mt("calculator", "calculator.log");
        impl_->customLogger->set_level(spdlog::level::debug);
        impl_->customLogger->flush_on(spdlog::level::err);
    }
    catch (const spdlog::spdlog_ex& e)
    {
        impl_->customLogger = spdlog::stderr_color_mt("err");
        impl_->customLogger->set_level(spdlog::level::debug);
        impl_->customLogger->flush_on(spdlog::level::err);
    }
}

calculator::Logger::~Logger() = default;

calculator::Logger& calculator::Logger::instance()
{
    static Logger logger;
    return logger;
}

void calculator::Logger::debug(const std::string& message)
{
    impl_->customLogger->debug(message);
}
void calculator::Logger::warning(const std::string& message)
{
    impl_->customLogger->warn(message);
}
void calculator::Logger::error(const std::string& message)
{
    impl_->customLogger->error(message);
}
void calculator::Logger::info(const std::string& message)
{
    impl_->customLogger->info(message);
}