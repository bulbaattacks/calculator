#include "app.h"

#include <getopt.h>
#include <unistd.h>

#include <cerrno>
#include <climits>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>

int calculator::Application::run(int argc, char** argv)
{
    try
    {
        makeTask(argc, argv, task_);
    }
    catch (const std::exception& e)
    {
        std::cerr << "Can not make task. " << e.what() << "\n";
        return EXIT_FAILURE;
    }

    if (task_.showHelp)
    {
        printHelp(argv[0]);
        return EXIT_SUCCESS;
    }

    try
    {
        makeCalculate(task_);
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error while calculating. " << e.what() << "\n";
        return EXIT_FAILURE;
    }

    printResult(task_);
    return EXIT_SUCCESS;
    // {
    //     return EXIT_SUCCESS;
    // }
    // return EXIT_FAILURE;
}

void calculator::Application::printHelp(const char* program)
{
    printf("Usage: %s -a NUM -o OP [-b NUM]\n", program);
    fputs(R"(
Integer calculator.

Options:
  -a NUM   first operand (int)
  -b NUM   second operand (int)
  -o OP    operation, one of: + - * / ^ !
  -h       show this help and exit

Operations:
  +   addition            a + b
  -   subtraction         a - b
  *   multiplication      a * b
  /   integer division    a / b, the fractional part is discarded
  ^   power               a ^ b, b must be >= 0
  !   factorial           a!, a must be >= 0

Examples:
  calculator-app -a 5 -b 10 -o +
  calculator-app -a 2 -b 10 -o '^'
  calculator-app -a 5 -o '!'
  calculator-app -a 6 -b 7 -o '*'
)",
          stdout);
}

void calculator::Application::parseInt(const char* str, int& value)
{
    char* end = nullptr;
    errno = 0;
    const int64_t parsed = strtol(str, &end, 10);
    if (end == str || *end != '\0' || errno == ERANGE || parsed < INT_MIN ||
        parsed > INT_MAX)
    {
        throw std::invalid_argument("No valid argument to parse");
    }
    value = static_cast<int>(parsed);
}

void calculator::Application::makeTask(int argc, char** argv, Task& task)
{
    while (true)
    {
        int opt = getopt(argc, argv, "a:b:o:h");

        if (opt == 'h')
        {
            task.showHelp = true;
            return;
        }

        if (opt == -1)
        {
            break;
        }

        if (opt == '?')
        {
            throw std::invalid_argument("Unknown argument. Use -h for help.");
        }

        if (opt == 'a')
        {
            parseInt(optarg, task.firstNumber);
            task.getFirstArgument = true;
        }

        if (opt == 'b')
        {
            parseInt(optarg, task.secondNumber);
            task.getSecondArgument = true;
        }

        if (opt == 'o')
        {
            if (strlen(optarg) != 1)
            {
                throw std::invalid_argument(
                    "Unknown operation: " + std::string(optarg) + ". " +
                    "Use -h for help.");
            }
            task.operation = *optarg;
        }
    }
    if (task.operation != '!')
    {
        if (!task.getFirstArgument || !task.getSecondArgument)
        {
            throw std::logic_error("Missing argument. Use -h for help.");
        }
    }
    else
    {
        if (!task.getFirstArgument)
        {
            throw std::logic_error("Enter first argument. Use -h for help.");
        }
    }
}

ar_math::Status calculator::Application::makeCalculate(Task& task)
{
    switch (task.operation)
    {
        case '+':
            task.status = ar_math::addition(task.firstNumber, task.secondNumber,
                                            task.result);
            if (task.status == ar_math::Status::Overflow)
            {
                throw std::overflow_error("Result is overflow.");
            }
            break;
        case '-':
            task.status = ar_math::subtraction(task.firstNumber,
                                               task.secondNumber, task.result);
            if (task.status == ar_math::Status::Overflow)
            {
                throw std::overflow_error("Result is overflow.");
            }
            break;
        case '*':
            task.status = ar_math::multiplication(
                task.firstNumber, task.secondNumber, task.result);
            if (task.status == ar_math::Status::Overflow)
            {
                throw std::overflow_error("Result is overflow.");
            }
            break;
        case '/':
            task.status = ar_math::division(task.firstNumber, task.secondNumber,
                                            task.result);
            if (task.status == ar_math::Status::Overflow)
            {
                throw std::overflow_error("Result is overflow.");
            }
            if (task.status == ar_math::Status::DivisionByZero)
            {
                throw std::domain_error("Division by zero is forbidden.");
            }
            break;
        case '^':
            task.status = ar_math::power(task.firstNumber, task.secondNumber,
                                         task.result);
            if (task.status == ar_math::Status::NegativeArgument)
            {
                throw std::domain_error("Negative argument is forbidden.");
            }
            if (task.status == ar_math::Status::Overflow)
            {
                throw std::overflow_error("Result is overflow.");
            }
            break;
        case '!':
            task.status = ar_math::factorial(task.firstNumber, task.result);
            if (task.status == ar_math::Status::NegativeArgument)
            {
                throw std::domain_error("Negative argument is forbidden.");
            }
            if (task.status == ar_math::Status::Overflow)
            {
                throw std::overflow_error("Result is overflow.");
            }
            break;
        default:
            task.status = ar_math::Status::UnknownOperation;
            throw std::logic_error("Unknown operation.");
    }
    return task.status;
}

void calculator::Application::printResult(Task& task)
{
    if (task.status == ar_math::Status::Ok)
    {
        printf("Result: %d\n", task.result);
    }
}
