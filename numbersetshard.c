#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

int* primesUpTo(int n, int* o_primeCount)
{
    if (n < 2)
    {
        *o_primeCount = 0;
        return malloc(sizeof(int));
    }
    int approxPi = ceil(1.5*n / log(n));
    int* primes = malloc(approxPi * sizeof(int));
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
    *o_primeCount = i;
    return primes;
}

int implBSearchCeil(int n, const int* arr, int start, int stop, int len)
{
    switch (stop - start)
    {
        case 0:
            return -1;
        case 1:
        {
            if (arr[start] == n)
                return start;
            else if (stop < len)
                return stop;
            else
                return -1;
        }
        default:
        {
            int mid = start + (stop - start)/2;
            if (n < arr[mid])
                return implBSearchCeil(n, arr, start, mid, len);
            else if (n > arr[mid])
                return implBSearchCeil(n, arr, mid, stop, len);
            else
                return mid;
        }
    }
}

#define BSEARCH_CEIL(n, arr, len) implBSearchCeil(n, arr, 0, len, len)

inline int findRoot(int n, int* sets)
{
    while (n != sets[n])
    {
        n = sets[n];
        sets[n] = sets[sets[n]];
    }
    return n;
}

bool join(int a, int b, int* restrict sets, int* restrict cardinalities)
{
    int aRoot = findRoot(a, sets), bRoot = findRoot(b, sets);
    if (aRoot != bRoot)
    {
        int minimum, maximum;
        if (cardinalities[aRoot] < cardinalities[bRoot])
            minimum = aRoot, maximum = bRoot;
        else
            minimum = bRoot, maximum = aRoot;
        cardinalities[maximum] += cardinalities[minimum];
        cardinalities[minimum] = 0;
        sets[minimum] = sets[maximum];
        return true;
    }
    else
        return false;
}

#define POSMOD(a, m) ((m + (a % m)) % m)

int setsRemaining(long long start, int len, int* restrict sets, int* restrict cardinalities, int primeIndex, int primeCount,
    const int* restrict primes)
{
    for (int i = 0; i < len; ++i)
        sets[i] = i, cardinalities[i] = 1;
    int remaining = len;
    for (int i = primeIndex; i < primeCount; ++i)
    {
        int p = primes[i];
        if (p >= len)
            break;
        int a = POSMOD(-start, p);
        for (int n = a + p; n < len; n += p)
            remaining -= join(a, n, sets, cardinalities);
    }
    return remaining;
}

int main(void)
{
    int caseCount;
    scanf("%d", &caseCount);
    struct
    {
        long long start;
        int len;
        int lowerBound;
    } input[caseCount];
    int results[caseCount];
    int maxLen = 0;
    for (int i = 0; i < caseCount; ++i)
    {
        long long start, end, lowerBound;
        scanf("%lld %lld %lld", &start, &end, &lowerBound);
        int len = (end - start) + 1;
        if (len < 2 || lowerBound > len)
            results[i] = len;
        else
        {
            input[i].start = start, input[i].len = len, input[i].lowerBound = lowerBound;
            results[i] = -1;
            if (len > maxLen)
                maxLen = len;
        }
    }
    if (maxLen)
    {
        int primeCount;
        const int* primes = primesUpTo(maxLen, &primeCount);
        int *sets = malloc(maxLen * sizeof(int)), *cardinalities = malloc(maxLen * sizeof(int));
        for (int i = 0; i < caseCount; ++i)
        {
            if (results[i] == -1)
            {
                int primeIndex = BSEARCH_CEIL(input[i].lowerBound, primes, primeCount);
                if (primeIndex == -1)
                    results[i] = input[i].len;
                else
                    results[i] =
                        setsRemaining(input[i].start, input[i].len, sets, cardinalities, primeIndex, primeCount, primes);
            }
        }
        free((int*)primes), free(sets), free(cardinalities);
    }
    for (int i = 0; i < caseCount; ++i)
        printf("Case #%d: %d\n", i + 1, results[i]);
    return 0;
}
