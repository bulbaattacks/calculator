#include "log.h"

#include <spdlog/common.h>
#include <spdlog/logger.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

#include <memory>

namespace
{
std::shared_ptr<spdlog::logger> fileLogger;
}

calculator::Logger::Logger()
{
    fileLogger = spdlog::basic_logger_mt("calculator", "calculator.log");
    fileLogger->set_level(spdlog::level::debug);
    fileLogger->flush_on(spdlog::level::err);
}

calculator::Logger& calculator::Logger::instance()
{
    static Logger logger;
    return logger;
}

void calculator::Logger::debug(const std::string& message)
{
    fileLogger->debug(message);
}
void calculator::Logger::warning(const std::string& message)
{
    fileLogger->warn(message);
}
void calculator::Logger::error(const std::string& message)
{
    fileLogger->error(message);
}
void calculator::Logger::info(const std::string& message)    { fileLogger->info(message); }