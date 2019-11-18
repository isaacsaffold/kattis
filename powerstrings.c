#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>

#define MAX_STRLEN 2000000
#define MAX_OMEGA 7

struct PrimePower
{
    int prime, power;
};

// `factors` should be zeroed out.
int factor(int n, struct PrimePower* factors)
{
    int power = 0, i = 0;
    while (!(n & 1))
        ++power, n >>= 1;
    if (power)
        factors[i++] = (struct PrimePower){2, power};
    for (int k = 3, lim = sqrt(n); n > 1 && k <= lim; k += 2)
    {
        power = 0;
        while (!(n % k))
            ++power, n /= k;
        if (power)
            factors[i++] = (struct PrimePower){k, power};
    }
    if (n > 1)
        factors[i++] = (struct PrimePower){n, 1};
    return i;
}

int intPow(int a, int b)
{
    switch (b)
    {
        case 0:
            return 1;
        case 1:
            return a;
        default:;
            int n = intPow(a, b >> 1);
            return b & 1 ? a*n*n : n*n;
    }
}

// `n` must divide `len`.
bool hasEqualSegments(const char* str, int len, int n)
{
    int subLen = len / n, offset = subLen;
    for (int i = 1; i < n; ++i)
    {
        if (memcmp(str, str + offset, subLen))
            return false;
        else
            offset += subLen;
    }
    return true;
}

int maxStringPower(const char* str, int len)
{
    if (!len)
        return 0;
    struct PrimePower factors[MAX_OMEGA];
    int maxPower = 1, omega = factor(len, factors);
    for (int i = 0; i < omega; ++i)
    {
        int v = 0;
        for (int j = 0; j < factors[i].power; ++j)
        {
            if (hasEqualSegments(str, len, factors[i].prime))
                ++v, len /= factors[i].prime;
            else
                break;
        }
        maxPower *= intPow(factors[i].prime, v);
    }
    return maxPower;
}

int main(void)
{
    char buf[MAX_STRLEN + 2];
    while (fgets(buf, MAX_STRLEN + 2, stdin)[0] != '.')
    {
        int len = strlen(buf);
        if (buf[len - 1] == '\n')
            --len;
        printf("%d\n", maxStringPower(buf, len));
    }
    return 0;
}
