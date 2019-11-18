#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

int gcd(int a, int b)
{
    while (b)
    {
        int mod = a % b;
        a = b, b = mod;
    }
    return a;
}

int primesUpTo(int n, int primes[])
{
    if (n < 2)
        return 0;
    bool* notPrime = calloc(n + 1, sizeof(bool));
    primes[0] = 2;
    for (int k = 4; k <= n; k += 2)
        notPrime[k] = true;
    int p = 3, i = 1;
    for (int lim = ceil(sqrt(n)); p <= lim; p += 2)
    {
        if (!notPrime[p])
        {
            primes[i++] = p;
            for (int k = p << 1; k <= n; k += p)
                notPrime[k] = true;
        }
    }
    for (; p <= n; p += 2)
    {
        if (!notPrime[p])
            primes[i++] = p;
    }
    free(notPrime);
    return i;
}

int minBase(const char str[])
{
    char maxChar = '\0';
    for (; *str; ++str)
        maxChar = *str > maxChar ? *str : maxChar;
    switch (maxChar)
    {
        case '0': case '1':
            return 2;
        case '2': case '3': case '4': case '5': case '6': case '7':
            return 8;
        case '8': case '9':
            return 10;
        case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
            return 16;
        default:
            return 0;
    }
}

void isPrime(const char digits[], int base, int o_prob[static restrict 2], const int primes[restrict], int primeCount)
{
    long long binForm, octForm, decForm, hexForm = strtoll(digits, NULL, 16);
    unsigned char primeInBase;
    if (hexForm < 2)
    {
        o_prob[0] = 0, o_prob[1] = 1;
        return;
    }
    int i = 0;
    switch (base)
    {
        case 2:
            binForm = strtoll(digits, NULL, 2);
            octForm = strtoll(digits, NULL, 8);
            decForm = strtoll(digits, NULL, 10);
            primeInBase = 15;
            o_prob[1] = 4;
            break;
        case 8:
            octForm = strtoll(digits, NULL, 8);
            decForm = strtoll(digits, NULL, 10);
            primeInBase = 7;
            o_prob[1] = 3;
            goto octal;
        case 10:
            decForm = strtoll(digits, NULL, 10);
            primeInBase = 3;
            o_prob[1] = 2;
            goto decimal;
        case 16:
            o_prob[1] = 1;
            primeInBase = 1;
            goto hexadecimal;
    }
    int binLim = lround(sqrt(binForm));
    for (; i < primeCount && primes[i] <= binLim; ++i)
    {
        int p = primes[i];
        if (!(binForm % p))
            primeInBase &= 7;
        if (!(octForm % p))
            primeInBase &= 11;
        if (!(decForm % p))
            primeInBase &= 13;
        if (!(hexForm % p))
            primeInBase &= 14;
        if (!primeInBase)
        {
            o_prob[0] = 0, o_prob[1] = 1;
            return;
        }
    }
    octal:;
    int octLim = lround(sqrt(octForm));
    for (; i < primeCount && primes[i] <= octLim; ++i)
    {
        int p = primes[i];
        if (!(octForm % p))
            primeInBase &= 11;
        if (!(decForm % p))
            primeInBase &= 13;
        if (!(hexForm % p))
            primeInBase &= 14;
        if (!primeInBase)
        {
            o_prob[0] = 0, o_prob[1] = 1;
            return;
        }
    }
    decimal:;
    int decLim = lround(sqrt(decForm));
    for (; i < primeCount && primes[i] <= decLim; ++i)
    {
        int p = primes[i];
        if (!(decForm % p))
            primeInBase &= 13;
        if (!(hexForm % p))
            primeInBase &= 14;
        if (!primeInBase)
        {
            o_prob[0] = 0, o_prob[1] = 1;
            return;
        }
    }
    hexadecimal:;
    int hexLim = lround(sqrt(hexForm));
    for (; i < primeCount && primes[i] <= hexLim; ++i)
    {
        int p = primes[i];
        if (!(hexForm % p))
            primeInBase &= 14;
        if (!primeInBase)
        {
            o_prob[0] = 0, o_prob[1] = 1;
            return;
        }
    }
    int numerator = 0;
    for (int i = 0; i < 4; ++i)
        numerator += (primeInBase >> i) & 1;
    int d = gcd(numerator, o_prob[1]);
    o_prob[0] = numerator / d, o_prob[1] /= d;
}

int main(void)
{
    int primes[100000], primeCount = primesUpTo(1 << 20, primes), caseCount;
    scanf("%d\n", &caseCount);
    for (int i = 0; i < caseCount; ++i)
    {
        int prob[2];
        char digits[12];
        fgets(digits, 12, stdin);
        int base = minBase(digits);
        isPrime(digits, base, prob, primes, primeCount);
        printf("%d/%d\n", prob[0], prob[1]);
    }
    return 0;
}
