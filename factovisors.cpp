#include <iostream>
#include <cmath>
#include <unordered_map>

using namespace std;

long long intPow(long long x, long long y)
{
    if (!y)
        return 1;
    else
    {
        long long n = intPow(x, y/2);
        return y % 2 ? x*n*n : n*n;
    }
}

int valuation(int k, int &n)
{
    int v = 0;
    for (; n % k == 0; ++v)
        n /= k;
    return v;
}

unordered_map<int, int> pfact(int n)
{
    unordered_map<int, int> factors;
    int v2 = valuation(2, n);
    if (v2)
        factors[2] = v2;
    int lim = static_cast<int>(sqrt(n)) + 1;
    for (int k = 3; k <= lim && n > 1; k += 2)
    {
        int v = valuation(k, n);
        if (v)
            factors[k] = v;
    }
    if (n > 1)
        factors[n] = 1;
    return factors;
}

// `fact` is an argument to the factorial function, rather than its result.
long long factorialValuation(int p, int fact)
{
    long long v = 0;
    for (int k = 0; fact; ++k)
    {
        long long mod = fact % p;
        fact /= p;
        if (mod)
            v += mod * (intPow(p, k) - 1) / (p - 1);
    }
    return v;
}

bool isFactorialDivisor(int n, int fact)
{
    if (!n)
        return false;
    for (const auto &elem: pfact(n))
    {
        if (elem.second > factorialValuation(elem.first, fact))
            return false;
    }
    return true;
}

int main()
{
    ios::sync_with_stdio(false);
    cin.tie(NULL);

    int fact, n;
    while (cin >> fact >> n)
    {
        const char *resultStr = isFactorialDivisor(n, fact) ? " divides " : " does not divide ";
        cout << n << resultStr << fact << "!\n";
    }

    return 0;
}
