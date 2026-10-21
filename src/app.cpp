#include "app.h"

#include <ar_math/math.h>
#include <getopt.h>
#include <unistd.h>

#include <cerrno>
#include <climits>
#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace
{

struct Task
{
    int firstNumber = 0;
    int secondNumber = 0;
    char operation = '\0';
    int result = 0;
    ar_math::Status status = ar_math::Status::InvalidArgument;
    bool showHelp = false;
    bool getFirstArgument = false;
    bool getSecondArgument = false;
};

void printHelp(const char* program)
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

bool parseInt(const char* str, int& value)
{
    char* end = nullptr;
    errno = 0;
    const long parsed = strtol(str, &end, 10);
    if (end == str || *end != '\0' || errno == ERANGE || parsed < INT_MIN ||
        parsed > INT_MAX)
    {
        return false;
    }
    value = static_cast<int>(parsed);
    return true;
}

bool makeTask(int argc, char** argv, Task& task)
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

ar_math::Status makeCalculate(Task& task)
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
            task.status = ar_math::Status::InvalidArgument;
    }
    return task.status;
}

void printResult(Task& task)
{
    if (task.status == ar_math::Status::Ok)
    {
        printf("Result: %d\n", task.result);
    }

    if (task.status == ar_math::Status::Overflow)
    {
        fprintf(stderr, "Error: result is overflow\n");
    }

    if (task.status == ar_math::Status::InvalidArgument)
    {
        fprintf(stderr, "Error: some argument is invalid. Use -h for help\n");
    }
}

} // namespace

int app::applicationRun(int argc, char** argv)
{
    Task task{};
    if (!makeTask(argc, argv, task))
    {
        return EXIT_FAILURE;
    }

    if (task.showHelp)
    {
        printHelp(argv[0]);
        return EXIT_SUCCESS;
    }

    makeCalculate(task);
    printResult(task);
    if (task.status == ar_math::Status::Ok)
    {
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}
