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
    int run(int argc, char** argv);

  private:
    void printHelp(const char* program);
    void parseInt(const char* str, int& value);
    void makeTask(int argc, char** argv, Task& task);
    ar_math::Status makeCalculate(Task& task);
    void printResult(Task& task);

  private:
    Task task_;
};

} // namespace calculator