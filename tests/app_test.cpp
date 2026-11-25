#include "app.h"

#include <cstdlib>
#include <string>
#include <vector>

#include <gtest/gtest.h>

namespace
{

struct RunResult
{
    int exitCode;
    std::string out;
    std::string err;
};

RunResult runApp(std::vector<std::string> args)
{
    args.insert(args.begin(), "calculator-app");

    std::vector<char*> argv;
    for (std::string& arg : args)
    {
        argv.push_back(arg.data());
    }
    argv.push_back(nullptr);

    calculator::Application app;

    testing::internal::CaptureStdout();
    testing::internal::CaptureStderr();
    const int code = app.run(static_cast<int>(args.size()), argv.data());

    return RunResult{code, testing::internal::GetCapturedStdout(),
                     testing::internal::GetCapturedStderr()};
}

} // namespace

TEST(ApplicationTest, AddsTwoPositiveNumbers)
{
    const RunResult result = runApp({R"({"a":2,"b":3,"o":"+"})"});
    EXPECT_EQ(result.exitCode, EXIT_SUCCESS);
    EXPECT_EQ(result.out, "Result: 5\n");
    EXPECT_TRUE(result.err.empty()) << "stderr: " << result.err;
}

TEST(ApplicationTest, SubtractsTwoPositiveNumbers)
{
    const RunResult result = runApp({R"({"a":3,"b":2,"o":"-"})"});
    EXPECT_EQ(result.exitCode, EXIT_SUCCESS);
    EXPECT_EQ(result.out, "Result: 1\n");
    EXPECT_TRUE(result.err.empty()) << "stderr: " << result.err;
}

TEST(ApplicationTest, MultiplicatsTwoPositiveNumbers)
{
    const RunResult result = runApp({R"({"a":3,"b":2,"o":"*"})"});
    EXPECT_EQ(result.exitCode, EXIT_SUCCESS);
    EXPECT_EQ(result.out, "Result: 6\n");
    EXPECT_TRUE(result.err.empty()) << "stderr: " << result.err;
}

TEST(ApplicationTest, DivisionsTwoPositiveNumbers)
{
    const RunResult result = runApp({R"({"a":2,"b":2,"o":"/"})"});
    EXPECT_EQ(result.exitCode, EXIT_SUCCESS);
    EXPECT_EQ(result.out, "Result: 1\n");
    EXPECT_TRUE(result.err.empty()) << "stderr: " << result.err;
}

TEST(ApplicationTest, PowersTwoPositiveNumbers)
{
    const RunResult result = runApp({R"({"a":2,"b":2,"o":"^"})"});
    EXPECT_EQ(result.exitCode, EXIT_SUCCESS);
    EXPECT_EQ(result.out, "Result: 4\n");
    EXPECT_TRUE(result.err.empty()) << "stderr: " << result.err;
}

TEST(ApplicationTest, FactorialsTwoPositiveNumbers)
{
    const RunResult result = runApp({R"({"a":2,"o":"!"})"});
    EXPECT_EQ(result.exitCode, EXIT_SUCCESS);
    EXPECT_EQ(result.out, "Result: 2\n");
    EXPECT_TRUE(result.err.empty()) << "stderr: " << result.err;
}

TEST(ApplicationTest, AdditionOverflowReportsError)
{
    const RunResult result = runApp({R"({"a":2147483647,"b":1,"o":"+"})"});
    EXPECT_EQ(result.exitCode, EXIT_FAILURE);
    EXPECT_EQ(result.err, "Error while calculating. Result is overflow.\n");
    EXPECT_FALSE(result.err.empty());
}

TEST(ApplicationTest, SubtractionOverflowReportsError)
{
    const RunResult result = runApp({R"({"a":-2147483648,"b":1,"o":"-"})"});
    EXPECT_EQ(result.exitCode, EXIT_FAILURE);
    EXPECT_EQ(result.err, "Error while calculating. Result is overflow.\n");
    EXPECT_FALSE(result.err.empty());
}

TEST(ApplicationTest, MultiplicationOverflowReportsError)
{
    const RunResult result = runApp({R"({"a":-2147483648,"b":2,"o":"*"})"});
    EXPECT_EQ(result.exitCode, EXIT_FAILURE);
    EXPECT_EQ(result.err, "Error while calculating. Result is overflow.\n");
    EXPECT_FALSE(result.err.empty());
}

TEST(ApplicationTest, DivisionOverflowReportsError)
{
    const RunResult result = runApp({R"({"a":-2147483648,"b":-1,"o":"/"})"});
    EXPECT_EQ(result.exitCode, EXIT_FAILURE);
    EXPECT_EQ(result.err, "Error while calculating. Result is overflow.\n");
    EXPECT_FALSE(result.err.empty());
}

TEST(ApplicationTest, PowerOverflowReportsError)
{
    const RunResult result = runApp({R"({"a":2147483647,"b":2,"o":"^"})"});
    EXPECT_EQ(result.exitCode, EXIT_FAILURE);
    EXPECT_EQ(result.err, "Error while calculating. Result is overflow.\n");
    EXPECT_FALSE(result.err.empty());
}

TEST(ApplicationTest, DivisionByZeroReportsError)
{
    const RunResult result = runApp({R"({"a":2147483647,"b":0,"o":"/"})"});
    EXPECT_EQ(result.exitCode, EXIT_FAILURE);
    EXPECT_EQ(result.err,
              "Error while calculating. Division by zero is forbidden.\n");
    EXPECT_FALSE(result.err.empty());
}

TEST(ApplicationTest, PowerInNegativeReportsError)
{
    const RunResult result = runApp({R"({"a":7,"b":-7,"o":"^"})"});
    EXPECT_EQ(result.exitCode, EXIT_FAILURE);
    EXPECT_EQ(result.err,
              "Error while calculating. Negative argument is forbidden.\n");
    EXPECT_FALSE(result.err.empty());
}

TEST(ApplicationTest, FactorialInNegativeReportsError)
{
    const RunResult result = runApp({R"({"a":-7,"o":"!"})"});
    EXPECT_EQ(result.exitCode, EXIT_FAILURE);
    EXPECT_EQ(result.err,
              "Error while calculating. Negative argument is forbidden.\n");
    EXPECT_FALSE(result.err.empty());
}

TEST(ApplicationTest, UnknownOperationReportsError)
{
    const RunResult result = runApp({R"({"a":7,"b":-7,"o":"%"})"});
    EXPECT_EQ(result.exitCode, EXIT_FAILURE);
    EXPECT_EQ(result.err, "Error while calculating. Unknown operation.\n");
    EXPECT_FALSE(result.err.empty());
}