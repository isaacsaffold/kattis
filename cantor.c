#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#define MAX(a, b) ((a) > (b) ? (a) : (b))

struct Fraction
{
    int num, denom;
};

int gcd(int a, int b)
{
    while (b)
    {
        int mod = a % b;
        a = b, b = mod;
    }
    return a;
}

int intPow(int a, int b)
{
    switch (b)
    {
        case 0:
            return 1;
        case 1:
            return a;
        default:
        {
            int n = intPow(a, b >> 1);
            return b & 1 ? a*n*n : n*n;
        }
    }
}

struct Fraction toFraction(const char* decimal)
{
    char* end;
    int intPart = strtol(decimal, &end, 10);
    struct Fraction r;
    if (*end == '.')
    {
        char* start = end + 1;
        r.num = strtol(start, &end, 10), r.denom = intPow(10, (end - start));
    }
    else
        r.num = 0, r.denom = 1;
    int d = gcd(r.num, r.denom);
    r.num /= d, r.denom /= d;
    r.num += intPart * r.denom;
    return r;
}

int valuation(int n, int p)
{
    int v = 0;
    for (; !(n % p); n /= p)
        ++v;
    return v;
}

bool inCantorSet(const char* decimal)
{
    struct Fraction r = toFraction(decimal);
    int v2 = valuation(r.denom, 2), v5 = valuation(r.denom, 5);
    int ordExp2 = 0, ordExp5 = 0;
    if (v2 && v5)
        ordExp2 = MAX(2, v2 - 2), ordExp5 = v5 - 1;
    else if (v2)
        ordExp2 = v2 - 1 - (v2 > 2);
    else if (v5)
        ordExp2 = 2, ordExp5 = v5 - 1;
    else
        return true;
    int ord = (1 << ordExp2) * intPow(5, ordExp5);
    for (int i = 0; i < ord; ++i)
    {
        if (r.num < r.denom)
            r.num *= 3;
        for (; r.num < r.denom; ++i)
            r.num *= 3;
        div_t divmod = div(r.num, r.denom);
        if (divmod.quot == 1)
            return false;
        else
            r.num = divmod.rem;
    }
    return true;
}

int main(void)
{
    char buf[10];
    while (true)
    {
        fgets(buf, 10, stdin);
        if (buf[0] == 'E')
            break;
        puts(inCantorSet(buf) ? "MEMBER" : "NON-MEMBER");
    }
    return 0;
}
