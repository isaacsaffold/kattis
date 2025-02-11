#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

bool magicBitString(int p, char* out)
{
    if (p == 2)
        return false;
    memset(out, '1', p - 1);
    out[p - 1] = '\0';
    for (long long i = 1, n = (p - 1)/2; i <= n; ++i)
        out[(i*i % p) - 1] = '0';
    return true;
}

int main(void)
{
    char* buf = malloc(100000);
    int n;
    while (true)
    {
        scanf("%d", &n);
        if (n == 0)
            break;
        if (magicBitString(n, buf))
            puts(buf);
        else
            puts("Impossible");
    }
    free(buf);
    return 0;
}
