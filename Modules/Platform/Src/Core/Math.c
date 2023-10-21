double clamp(double value, double lower, double upper)
{
    if (value < lower)
    {
        return lower;
    }

    if (upper < value)
    {
        return upper;
    }

    return value;
}

unsigned long long
clampUnsigned(unsigned long long value, unsigned long long lower, unsigned long long upper)
{
    if (value < lower)
    {
        return lower;
    }

    if (upper < value)
    {
        return upper;
    }

    return value;
}
