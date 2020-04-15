#include <stdio.h>

#define MODULUS 1000000007
#define MAX_STRLEN 500000

int modPow(int base, int exp, int mod)
{
    switch (exp)
    {
        case 0:
            return mod > 1;
        case 1:
            return base % mod;
        default:;
            long long n = modPow(base, exp >> 1, mod);
            long long nn = n*n % mod;
            return exp & 1 ? base*nn % mod : nn;
    }
}

int inversionCount(const char* bits)
{
    int total = 0;
    int numOnes = 0, numQuestionMarks = 0;
    for (; bits; ++bits)
    {
        switch (*bits)
        {
            case '0':
                total = (total + 4*numOnes + 2*numQuestionMarks) % MODULUS;
                break;
            case '1':
                ++numOnes;
                break;
            case '?':
                total = (total + 2*numOnes + numQuestionMarks++) % MODULUS;
                break;
            default:
                goto afterLoop;
        }
    }
    afterLoop:;
    total = (long long)total * modPow(MODULUS >> 1, 2, MODULUS) % MODULUS;
    return (long long)total * modPow(2, numQuestionMarks, MODULUS) % MODULUS;
}

int main(void)
{
    char buf[MAX_STRLEN + 2];
    fgets(buf, MAX_STRLEN + 2, stdin);
    printf("%d", inversionCount(buf));
    return 0;
}
