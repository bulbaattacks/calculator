#pragma once

#include "nlohmann/json_fwd.hpp"

#include <ar_math/math.h>

namespace calculator
{

struct Task
{
    int firstNumber = 0;
    int secondNumber = 0;
    char operation = '\0';
    int result = 0;
    ar_math::Status status = ar_math::Status::InitialStatus;
    bool showHelp = false;
    bool getFirstArgument = false;
    bool getSecondArgument = false;
};

class Application
{
  public:
    int run(int argc, char** argv);

  private:
    nlohmann::json parseline(int argc, char** argv);
    void printHelp(const char* program);
    int parseInt(const nlohmann::json& argument);
    void makeTask(const nlohmann::json& json, Task& task);
    ar_math::Status makeCalculate(Task& task);
    void printResult(Task& task);

  private:
    Task task_;
};

} // namespace calculator