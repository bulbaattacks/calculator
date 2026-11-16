#pragma once

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
    int applicationRun(int argc, char** argv);

  private:
    void printHelp(const char* program);
    bool parseInt(const char* str, int& value);
    bool makeTask(int argc, char** argv, Task& task);
    ar_math::Status makeCalculate(Task& task);
    void printResult(Task& task);

  private:
    Task task_;
};

} // namespace calculator