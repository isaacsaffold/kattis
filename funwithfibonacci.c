#ifndef ITER_FIB_GRAPH_H
#define ITER_FIB_GRAPH_H

struct IndexInfo;
struct Vertex;

struct IterFibGraph
{
    int modulus;
    void* buf;
    int* restrict fibSeq;
    struct IndexInfo* restrict indices;
    struct Vertex* restrict vertices;
    int order;
    int id;
};

void ifg_init(struct IterFibGraph* ifg, int modulus);
void ifg_reset(struct IterFibGraph* ifg);
int ifg_iterFibMod(struct IterFibGraph* ifg, long long k, long long n);

#endif

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

struct IndexInfo
{
    int index;
    int id;
};

struct Vertex
{
    bool inCycle;
    int value;
    union
    {
        struct
        {
            int start;
            int cycleLength;
        };
        struct
        {
            int next;
            int pathLength;
        };
    };
};

void ifg_init(struct IterFibGraph* ifg, int modulus)
{
    ifg->modulus = modulus;
    char* buf = malloc(modulus * (sizeof(int) + sizeof(struct IndexInfo) + sizeof(struct Vertex)));
    ifg->buf = buf;
    ifg->fibSeq = (int*)buf;
    int a = 0, b = 1;
    for (int i = 0; i < modulus; ++i)
    {
        ifg->fibSeq[i] = a;
        int temp = (a + b) % modulus;
        a = b, b = temp;
    }
    buf += modulus * sizeof(int);
    ifg->indices = (struct IndexInfo*)buf;
    memset(ifg->indices, -1, modulus * sizeof(struct IndexInfo));
    buf += modulus * sizeof(struct IndexInfo);
    ifg->vertices = (struct Vertex*)buf;
}

void ifg_reset(struct IterFibGraph* ifg)
{
    free(ifg->buf);
    memset(ifg, 0, sizeof(struct IterFibGraph));
}

static int p_iterFibMod(const struct IterFibGraph* ifg, long long k, int n)
{
    int i = ifg->indices[n].index;
    while (!ifg->vertices[i].inCycle)
    {
        int len = ifg->vertices[i].pathLength;
        if (k <= len)
            return ifg->vertices[i + k].value;
        else
        {
            k -= len + 1;
            i = ifg->vertices[i].next;
        }
    }
    int start = ifg->vertices[i].start;
    return ifg->vertices[start + (i - start + k) % ifg->vertices[i].cycleLength].value;
}

int ifg_iterFibMod(struct IterFibGraph* ifg, long long k, long long n)
{
    int result = -1;
    int m = ifg->modulus, id = ifg->id++;
    int start = n % m, offset = ifg->order;
    int r = start, i = 0;
    for (; ifg->indices[r].id == -1; ++i)
    {
        ifg->indices[r] = (struct IndexInfo){offset + i, id};
        r = ifg->fibSeq[r];
        if (!--k)
            result = r;
    }
    if (ifg->indices[r].id == id)
    {
        int cycleStart = ifg->indices[r].index;
        int cycleLength = offset + i - cycleStart;
        for (struct Vertex *v = ifg->vertices + cycleStart, *end = v + cycleLength; v < end; ++v)
        {
            *v = (struct Vertex){true, r, .start = cycleStart, .cycleLength = cycleLength};
            r = ifg->fibSeq[r];
        }
        i -= cycleLength, ifg->order += cycleLength;
    }
    int next = ifg->indices[r].index;
    r = start;
    ifg->order += i;
    for (struct Vertex* v = ifg->vertices + offset; --i >= 0; ++v)
    {
        *v = (struct Vertex){false, r, .next = next, .pathLength = i};
        r = ifg->fibSeq[r];
    }
    if (result == -1)
        result = p_iterFibMod(ifg, k, r);
    return result;
}

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAX_OMEGA 8
#define MAX_MOD 9375000
#define MAX_LEO_LOG 9

struct PrimePower
{
    int prime;
    int exponent;
};

static const int identity[] = {1, 0, 1};
static const int phi[] = {1, 1, 0};

static int* restrict g_pisanoCache;
static struct IterFibGraph g_graphs[MAX_LEO_LOG] = {0};

#define SYM_MAT_EQ(a, b) ((a)[0] == (b)[0] && (a)[1] == (b)[1] && (a)[2] == (b)[2])
#define SYM_MAT_ASSIGN(a, b) ((a)[0] = (b)[0], (a)[1] = (b)[1], (a)[2] = (b)[2])
#define SYM_MAT_ASSIGN_MOD(a, b, m) ((a)[0] = (b)[0] % m, (a)[1] = (b)[1] % m, (a)[2] = (b)[2] % m)

static int gcd(int a, int b)
{
    while (b)
    {
        int mod = a % b;
        a = b, b = mod;
    }
    return a;
}

static int lcm(int a, int b)
{
    return (a / gcd(a, b)) * b;
}

static int intPow(int a, int b)
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

static int factorize(int n, struct PrimePower factors[static MAX_OMEGA])
{
    int i = 0, v = 0;
    for (; !(n & 1); n >>= 1)
        ++v;
    if (v)
        factors[i++] = (struct PrimePower){2, v};
    int lim = sqrt(n);
    for (int k = 3; k <= lim && n > 1; k += 2)
    {
        v = 0;
        for (; !(n % k); n /= k)
            ++v;
        if (v)
            factors[i++] = (struct PrimePower){k, v};
    }
    if (n > 1)
        factors[i++] = (struct PrimePower){n, 1};
    return i;
}

static void symMatPowMod(const int a[static restrict 3], int n, int m, int out[static restrict 3])
{
    switch (n)
    {
        case 0:
            SYM_MAT_ASSIGN_MOD(out, identity, m);
            break;
        case 1:
            SYM_MAT_ASSIGN_MOD(out, a, m);
            break;
        default:
            symMatPowMod(a, n >> 1, m, out);
            long long temp[] = {out[0], out[1], out[2]};
            long long s = temp[1]*temp[1];
            out[0] = (temp[0]*temp[0] + s) % m;
            out[1] = temp[1] * (temp[0] + temp[2]) % m;
            out[2] = (s + temp[2]*temp[2]) % m;
            if (n & 1)
            {
                SYM_MAT_ASSIGN(temp, out);
                out[0] = (a[0]*temp[0] + a[1]*temp[1]) % m;
                out[1] = (a[0]*temp[1] + a[1]*temp[2]) % m;
                out[2] = (a[1]*temp[1] + a[2]*temp[2]) % m;
            }
            break;
    }
}

static int fibMod(int n, int m)
{
    static int out[3];
    symMatPowMod(phi, n, m, out);
    return out[1];
}

static int primePisano(int p)
{
    int maxPi;
    switch (p % 5)
    {
        case 0:
            return 20;
        case 1:
        case 4:
            maxPi = p - 1;
            break;
        case 2:
            if (p == 2)
                return 3;
        case 3:
            maxPi = 2*(p + 1);
            break;
    }
    static struct PrimePower factors[MAX_OMEGA];
    int omega = factorize(maxPi, factors);
    int pi = 1;
    int symA[3], symB[3];
    for (int i = 0; i < omega; ++i)
    {
        int d = maxPi / intPow(factors[i].prime, factors[i].exponent);
        SYM_MAT_ASSIGN(symA, phi);
        symMatPowMod(symA, d, p, symB);
        int *in = symB, *out = symA;
        int j = 0;
        for (; !SYM_MAT_EQ(in, identity); ++j)
        {
            symMatPowMod(in, factors[i].prime, p, out);
            int* temp = in;
            in = out, out = temp;
        }
        pi *= intPow(factors[i].prime, j);
    }
    return pi;
}

static int pisano(int n)
{
    if (g_pisanoCache[n])
        return g_pisanoCache[n];
    else
    {
        static struct PrimePower factors[MAX_OMEGA];
        int omega = factorize(n, factors);
        int pi = 1;
        for (int i = 0; i < omega; ++i)
            pi = lcm(pi, intPow(factors[i].prime, factors[i].exponent - 1) * primePisano(factors[i].prime));
        g_pisanoCache[n] = pi;
        return pi;
    }
}

static int iterFibMod(long long k, long long n, int m)
{
    if (m == 1)
        return 0;
    int pi = pisano(m);
    if (k == 1)
        return fibMod(n % pi, m);
    else if (pi != m)
        return fibMod(iterFibMod(k - 1, n, pi), m);
    else
    {
        int v5 = 0;
        for (; !(m % 5); m /= 5, ++v5);
        struct IterFibGraph* graph = g_graphs + v5;
        if (!graph->modulus)
            ifg_init(graph, pi);
        return ifg_iterFibMod(graph, k, n);
    }
}

int main(void)
{
    g_pisanoCache = calloc(MAX_MOD, sizeof(int));
    int numCases;
    scanf("%d\n", &numCases);
    for (int i = 0; i < numCases; ++i)
    {
        long long n, k;
        int m;
        scanf("%lld %lld %d\n", &n, &k, &m);
        printf("%d\n", iterFibMod(k, n, m));
    }
    free(g_pisanoCache);
    for (int i = 0; i < sizeof g_graphs / sizeof(g_graphs[0]); ++i)
        ifg_reset(g_graphs + i);
    return 0;
}
