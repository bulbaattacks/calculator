#include "app.h"

#include "math.h"

#include <getopt.h>
#include <unistd.h>

#include <iostream>

namespace
{

struct Task
{
    int firstNumber;
    int secondNumber;
    char operation;
    int result;
    math::Status status;
};

int parseInt(const char* str)
{
    if (str == nullptr || *str == '\0')
    {
        return 0;
    }

    int result = 0;
    int sign = 1;
    int i = 0;

    if (str[0] == '-')
    {
        sign = -1;
        i = 1;
    }
    else if (str[0] == '+')
    {
        i = 1;
    }

    while (str[i] >= '0' && str[i] <= '9')
    {
        result = (result * 10) + (str[i] - '0');
        i++;
    }
    return result * sign;
}

void makeTask(int argc, char** argv, Task& task)
{
    while (true)
    {
        int opt = getopt(argc, argv, "a:b:o:");

        if (opt == -1)
        {
            break;
        }

        if (opt == 'a')
        {
            task.firstNumber = parseInt(optarg);
        }

        if (opt == 'b')
        {
            task.secondNumber = parseInt(optarg);
        }

        if (opt == 'o')
        {
            task.operation = *optarg;
        }
    }
}

math::Status makeCalculate(Task& task)
{
    switch (task.operation)
    {
        case '+':
            task.status = math::addition(task.firstNumber, task.secondNumber,
                                         task.result);
            break;
        case '-':
            task.status = math::subtraction(task.firstNumber, task.secondNumber,
                                            task.result);
            break;
        case '*':
            task.status = math::multiplication(task.firstNumber,
                                               task.secondNumber, task.result);
            break;
        case '/':
            task.status = math::division(task.firstNumber, task.secondNumber,
                                         task.result);
            break;
        case '^':
            task.status =
                math::power(task.firstNumber, task.secondNumber, task.result);
            break;
        case '!':
            task.status = math::factorial(task.firstNumber, task.result);
            break;
        default:
            task.status = math::Status::InvalidArgument;
    }
    return task.status;
}

void printResult(Task& task)
{
    if (task.status == math::Status::Ok)
    {
        std::cout << "Result: " << task.result << '\n';
    }

    if (task.status == math::Status::Overflow)
    {
        std::cout << "Error: result is overflow" << '\n';
    }

    if (task.status == math::Status::InvalidArgument)
    {
        std::cout << "Error: some argument is invalid" << '\n';
    }
}

} // namespace

void app::applicationRun(int argc, char** argv)
{
    Task task{};
    makeTask(argc, argv, task);
    makeCalculate(task);
    printResult(task);
}
