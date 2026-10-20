#include "app.h"

#include <ar_math/math.h>
#include <getopt.h>
#include <unistd.h>
#include <cstdio>

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
        int opt = getopt(argc, argv, "a:b:o:h");

        if (opt == -1)
        {
            break;
        }

        if (opt == 'h')
        {
            task.showHelp = true;
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

void app::applicationRun(int argc, char** argv)
{
    Task task{};
    makeTask(argc, argv, task);

    if (task.showHelp)
    {
        printHelp(argv[0]);
        return;
    }

    makeCalculate(task);
    printResult(task);
}
