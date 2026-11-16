#include "app.h"

#include <getopt.h>
#include <unistd.h>

#include <cerrno>
#include <climits>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

int calculator::Application::applicationRun(int argc, char** argv)
{
    if (!makeTask(argc, argv, task_))
    {
        return EXIT_FAILURE;
    }

    if (task_.showHelp)
    {
        printHelp(argv[0]);
        return EXIT_SUCCESS;
    }

    makeCalculate(task_);
    printResult(task_);
    if (task_.status == ar_math::Status::Ok)
    {
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
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

bool calculator::Application::parseInt(const char* str, int& value)
{
    char* end = nullptr;
    errno = 0;
    const int64_t parsed = strtol(str, &end, 10);
    if (end == str || *end != '\0' || errno == ERANGE || parsed < INT_MIN ||
        parsed > INT_MAX)
    {
        return false;
    }
    value = static_cast<int>(parsed);
    return true;
}

bool calculator::Application::makeTask(int argc, char** argv, Task& task)
{
    while (true)
    {
        int opt = getopt(argc, argv, "a:b:o:h");

        if (opt == 'h')
        {
            task.showHelp = true;
            return true;
        }

        if (opt == -1)
        {
            break;
        }

        if (opt == '?')
        {
            fprintf(stderr, "Wrong argument. Use -h for help\n");
            return false;
        }

        if (opt == 'a')
        {
            if (!parseInt(optarg, task.firstNumber))
            {
                fprintf(stderr, "Invalid argument: %s. Use -h for help\n",
                        optarg);
                return false;
            }
            task.getFirstArgument = true;
        }

        if (opt == 'b')
        {
            if (!parseInt(optarg, task.secondNumber))
            {
                fprintf(stderr, "Invalid argument: %s. Use -h for help\n",
                        optarg);
                return false;
            }
            task.getSecondArgument = true;
        }

        if (opt == 'o')
        {
            if (strlen(optarg) != 1)
            {
                fprintf(stderr, "Invalid operation: %s. Use -h for help\n",
                        optarg);
                return false;
            }
            task.operation = *optarg;
        }
    }
    if (task.operation != '!')
    {
        if (!task.getFirstArgument || !task.getSecondArgument)
        {
            fprintf(stderr, "Missing argument. Use -h for help\n");
            return false;
        }
    }
    else
    {
        if (!task.getFirstArgument)
        {
            fprintf(stderr, "Missing argument. Use -h for help\n");
            return false;
        }
    }
    return true;
}

ar_math::Status calculator::Application::makeCalculate(Task& task)
{
    switch (task.operation)
    {
        case '+':
            task.status = ar_math::addition(task.firstNumber, task.secondNumber,
                                            task.result);
            break;
        case '-':
            task.status = ar_math::subtraction(task.firstNumber,
                                               task.secondNumber, task.result);
            break;
        case '*':
            task.status = ar_math::multiplication(
                task.firstNumber, task.secondNumber, task.result);
            break;
        case '/':
            task.status = ar_math::division(task.firstNumber, task.secondNumber,
                                            task.result);
            break;
        case '^':
            task.status = ar_math::power(task.firstNumber, task.secondNumber,
                                         task.result);
            break;
        case '!':
            task.status = ar_math::factorial(task.firstNumber, task.result);
            break;
        default:
            task.status = ar_math::Status::UnknownOperation;
    }
    return task.status;
}

void calculator::Application::printResult(Task& task)
{
    if (task.status == ar_math::Status::Ok)
    {
        printf("Result: %d\n", task.result);
    }

    if (task.status == ar_math::Status::Overflow)
    {
        fprintf(stderr, "Error: result is overflow.\n");
    }

    if (task.status == ar_math::Status::DivisionByZero)
    {
        fprintf(stderr, "Error: Division by zero is forbidden.\n");
    }

    if (task.status == ar_math::Status::NegativeArgument)
    {
        fprintf(stderr, "Error: Negative argument is forbidden.\n");
    }

    if (task.status == ar_math::Status::UnknownOperation)
    {
        fprintf(stderr, "Error: Unknown operation. Use -h for help\n");
    }
}
