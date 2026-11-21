#pragma once

#include <string>

namespace calculator
{

class Logger
{
  public:
    static Logger& instance();

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    void debug(const std::string& message);
    void warning(const std::string& message);
    void error(const std::string& message);
    void info(const std::string& message);

  private:
    Logger();
};
} // namespace calculator