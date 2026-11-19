#include "app.h"

#include "nlohmann/json_fwd.hpp"

#include <getopt.h>
#include <unistd.h>

#include <nlohmann/json.hpp>

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
    if (argc < 2 || std::strcmp(argv[1], "--help") == 0)
    {
        printHelp(argv[0]);
        return EXIT_SUCCESS;
    }

    try
    {
        nlohmann::json json = parseline(argc, argv);
        makeTask(json, task_);
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error while building task. " << e.what() << "\n";
        return EXIT_FAILURE;
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
}

nlohmann::json calculator::Application::parseline(int argc, char** argv)
{
    if (argc < 2)
    {
        throw std::invalid_argument("Nothing to parse.");
    }
    nlohmann::json json = nlohmann::json::parse(argv[1]);
    return json;
}

void calculator::Application::printHelp(const char* program)
{
    printf("Usage: %s '{\"a\" : NUM, \"b\": NUM, \"o\": OP}'", program);
    fputs(R"(
Integer calculator.

Options:
  a NUM   first operand (int)
  b NUM   second operand (int)
  o OP    operation, one of: + - * / ^ !
  --help       show this help and exit

Operations:
  +   addition            a + b
  -   subtraction         a - b
  *   multiplication      a * b
  /   integer division    a / b, the fractional part is discarded
  ^   power               a ^ b, b must be >= 0
  !   factorial           a!, a must be >= 0

Examples:
  calculator-app '{"a":22, "b":2, "o":"+"}'
  calculator-app '{"a":2, "b":1, "o":"^"}'
  calculator-app '{"a":5, "o":"!"}'
  calculator-app '{"a":6, "b":7, "o":"*"}' 
)",
          stdout);
}

int calculator::Application::parseInt(const nlohmann::json& argument)
{
    if (!argument.is_number_integer())
    {
        throw std::invalid_argument("Argument must be integer: " +
                                    nlohmann::to_string(argument));
    }

    if (argument.is_number_unsigned())
    {
        if (argument.get<std::uint64_t>() > INT_MAX)
        {
            throw std::invalid_argument(
                "Argument is not valid: excess max int: " +
                nlohmann::to_string(argument));
        }
    }
    std::int64_t intArgument = argument.get<std::int64_t>();
    if (intArgument > INT_MAX)
    {
        throw std::invalid_argument("Argument is not valid: excess max int: " +
                                    nlohmann::to_string(argument));
    }
    if (intArgument < INT_MIN)
    {
        throw std::invalid_argument(
            "Argument is not valid: below the min int: " +
            nlohmann::to_string(argument));
    }
    return static_cast<int>(intArgument);
}

void calculator::Application::makeTask(const nlohmann::json& json, Task& task)
{

    std::string operationArgumentStr = json.at("o").get<std::string>();
    if (operationArgumentStr.length() != 1)
    {
        throw std::invalid_argument(
            "Unknown operation: " + operationArgumentStr + ". " +
            "Use --help for help.");
    }
    task.operation = operationArgumentStr[0];

    nlohmann::json firstArgument = json.at("a");
    task.firstNumber = parseInt(firstArgument);

    if (task.operation != '!')
    {
        nlohmann::json secondArgument = json.at("b");
        task.secondNumber = parseInt(secondArgument);
    }
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

    switch (task.status)
    {
        case ar_math::Status::Ok:
            return task.status;
        case ar_math::Status::NegativeArgument:
            throw std::domain_error("Negative argument is forbidden.");
        case ar_math::Status::Overflow:
            throw std::overflow_error("Result is overflow.");
        case ar_math::Status::DivisionByZero:
            throw std::domain_error("Division by zero is forbidden.");
        case ar_math::Status::UnknownOperation:
            throw std::logic_error("Unknown operation.");
        case ar_math::Status::InitialStatus:
            throw std::logic_error("Cant figure out status.");
        default:
            throw std::logic_error("Unknown status.");
    }
}

void calculator::Application::printResult(Task& task)
{
    if (task.status == ar_math::Status::Ok)
    {
        printf("Result: %d\n", task.result);
    }
}
