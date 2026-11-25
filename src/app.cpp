#include "app.h"

#include "log.h"

#include <nlohmann/json.hpp>

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
    calculator::Logger& logger = calculator::Logger::instance();

    if (argc < 2)
    {
        logger.warning("No json to parse. Help info is shown for a user.");
        printHelp(argv[0]);
        return EXIT_FAILURE;
    }
    if (std::strcmp(argv[1], "--help") == 0)
    {
        logger.info("User asks for help info.");
        printHelp(argv[0]);
        return EXIT_SUCCESS;
    }

    try
    {
        logger.info("Json input: " + std::string(argv[1]));
        nlohmann::json json = parseline(argc, argv);
        makeTask(json);
    }
    catch (const std::exception& e)
    {
        logger.error("Error while building task: " + std::string(e.what()));
        std::cerr << "Error while building task. " << e.what() << "\n";
        return EXIT_FAILURE;
    }

    try
    {
        logger.info("Start making calculation ...");
        logger.debug("First argument: " + std::to_string(task_.firstNumber));
        if (task_.operation != '!')
        {
            logger.debug("Second argument: " +
                         std::to_string(task_.secondNumber));
        }
        logger.debug("Operation argument: " + std::string(1, task_.operation));

        makeCalculate();
    }
    catch (const std::exception& e)
    {
        logger.error("Error while calculating: " + std::string(e.what()));
        std::cerr << "Error while calculating. " << e.what() << "\n";
        return EXIT_FAILURE;
    }

    logger.info("Calculation is ready. Result: " +
                std::to_string(task_.result));
    printResult();
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

void calculator::Application::makeTask(const nlohmann::json& json)
{

    std::string operationArgumentStr = json.at("o").get<std::string>();
    if (operationArgumentStr.length() != 1)
    {
        throw std::invalid_argument(
            "Unknown operation: " + operationArgumentStr + ". " +
            "Use --help for help.");
    }
    task_.operation = operationArgumentStr[0];

    nlohmann::json firstArgument = json.at("a");
    task_.firstNumber = parseInt(firstArgument);

    if (task_.operation != '!')
    {
        nlohmann::json secondArgument = json.at("b");
        task_.secondNumber = parseInt(secondArgument);
    }
}

void calculator::Application::makeCalculate()
{
    switch (task_.operation)
    {
        case '+':
            task_.status = ar_math::addition(task_.firstNumber,
                                             task_.secondNumber, task_.result);
            break;
        case '-':
            task_.status = ar_math::subtraction(
                task_.firstNumber, task_.secondNumber, task_.result);
            break;
        case '*':
            task_.status = ar_math::multiplication(
                task_.firstNumber, task_.secondNumber, task_.result);
            break;
        case '/':
            task_.status = ar_math::division(task_.firstNumber,
                                             task_.secondNumber, task_.result);
            break;
        case '^':
            task_.status = ar_math::power(task_.firstNumber, task_.secondNumber,
                                          task_.result);
            break;
        case '!':
            task_.status = ar_math::factorial(task_.firstNumber, task_.result);
            break;
        default:
            task_.status = ar_math::Status::UnknownOperation;
    }

    switch (task_.status)
    {
        case ar_math::Status::Ok:
            return;
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

void calculator::Application::printResult() const
{
    if (task_.status == ar_math::Status::Ok)
    {
        printf("Result: %d\n", task_.result);
    }
}
