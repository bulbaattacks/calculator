#pragma once

namespace math
{

enum class Status
{
    Ok = 0,
    InvalidArgument = -1,
    Overflow = -2,
};

Status addition(int a, int b, int& result);

Status subtraction(int a, int b, int& result);

Status division(int a, int b, int& result);

Status factorial(int a, int& result);

Status multiplication(int a, int b, int& result);

Status power(int a, int b, int& result);

} // namespace math
