#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define MAX_LEN 1000000

int invFactorial(const char* digits, int len)
{
    if (len <= 3)
    {
        switch (strtol(digits, NULL, 10))
        {
            case 1:
                return 1;
            case 2:
                return 2;
            case 6:
                return 3;
            case 24:
                return 4;
            case 120:
                return 5;
            case 720:
                return 6;
            default:
                // invalid input
                return 0;
        }
    }
    else
    {
        double sum = 0;
        int k = 1;
        while (sum < len)
            sum += log10(++k);
        return k - 1;
    }
}

int main(void)
{
    char* buf = malloc(MAX_LEN + 1);
    int len = fread(buf, 1, MAX_LEN, stdin);
    if (buf[len - 1] == '\n')
        --len;
    buf[len] = 0;
    printf("%d\n", invFactorial(buf, len));
    free(buf);
    return 0;
}
