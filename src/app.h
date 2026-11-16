#pragma once

#include <ar_math/math.h>

namespace calculator
{

struct Task;

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
};

} // namespace calculator