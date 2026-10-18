#include "math.h"

#include <climits>

math::Status math::addition(int a, int b, int& result)
{
    if (a > 0 && b > 0)
    {
        if (a > INT_MAX - b)
        {
            return Status::Overflow;
        }
    }

    if (a < 0 && b < 0)
    {
        if (a < INT_MIN - b)
        {
            return Status::Overflow;
        }
    }

    result = a + b;
    return Status::Ok;
}

math::Status math::subtraction(int a, int b, int& result)
{

    if (b < 0)
    {
        if (a > INT_MAX + b)
        {
            return Status::Overflow;
        }
    }

    if (b > 0)
    {
        if (a < INT_MIN + b)
        {
            return Status::Overflow;
        }
    }

    result = a - b;
    return Status::Ok;
}

math::Status math::division(int a, int b, int& result)
{
    if (b == 0) 
    {
        return Status::InvalidArgument;
    }

    if (a == INT_MIN && b == -1)
    {
        return Status::Overflow;
    }
    result = a / b;
    return Status::Ok;
}

math::Status math::multiplication(int a, int b, int& result)
{

    if (a > 0 && b > 0)
    {
        if (a > INT_MAX / b)
        {
            return Status::Overflow;
        }
    }

    if (a < 0 && b < 0)
    {
        if (a < INT_MAX / b)
        {
            return Status::Overflow;
        }
    }

    if (a < 0 && b > 0)
    {
        if (a < INT_MIN / b)
        {
            return Status::Overflow;
        }
    }

    if (a > 0 && b < 0)
    {
        if (b < INT_MIN / a)
        {
            return Status::Overflow;
        }
    }

    result = a * b;
    return Status::Ok;
}

math::Status math::factorial(int a, int& result)
{
    if (a < 0)
    {
        return Status::InvalidArgument;
    }

    if (a <= 1)
    {
        result = 1;
        return Status::Ok;
    }

    int previousResult = 0;
    Status status = factorial(a - 1, previousResult);

    if (status != Status::Ok)
    {
        return status;
    }

    if (a > INT_MAX / previousResult)
    {
        return Status::Overflow;
    }
    result = a * previousResult;
    return Status::Ok;
}

math::Status math::power(int a, int b, int& result)
{
    if (b < 0)
    {
        return Status::InvalidArgument;
    }

    if (a == INT_MAX && b > 1)
    {
        return Status::Overflow;
    }

    result = 1;
    for (int i = 1; i <= b; i++)
    {
        if (a > 0)
        {
            if (a > INT_MAX / result)
            {
                return Status::Overflow;
            }
        }

        if (a < 0)
        {
            if (result > 0)
            {
                if (a < INT_MIN / result)
                {
                    return Status::Overflow;
                }
            }
            if (result < 0)
            {
                if (a < INT_MAX / result)
                {
                    return Status::Overflow;
                }
            }
        }
        result *= a;
    }

    return Status::Ok;
}