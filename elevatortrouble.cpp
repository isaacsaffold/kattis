#include <iostream>
#include <array>
#include <cstdlib>
#include <algorithm>
#include <string>

using std::string_literals::operator""s;

constexpr int k_impossible = -1;

/*
 * Uses the extended Euclidean algorithm to find a solution to `ax + by = n`, where `n = gcd(a, b)`. The returned array
 * contains `x`, `y`, `n`, `a/n`, and `b/n`, in that order.
 */
std::array<int, 5> egcd(int a, int b)
{
    int r[2]{a, b}, s[2]{1, 0}, t[2]{0, 1};
    while (r[1])
    {
        int div = r[0] / r[1], mod = r[0] % r[1];
        int tempS = s[0] - div*s[1], tempT = t[0] - div*t[1];
        r[0] = r[1], r[1] = mod;
        s[0] = s[1], s[1] = tempS;
        t[0] = t[1], t[1] = tempT;
    }
    return std::array<int, 5>{s[0], t[0], r[0], std::abs(t[1]), std::abs(s[1])};
}

std::array<int, 2> minimalSolution(const std::array<int, 5>& bezout, int offset)
{
    long long c = offset / bezout[2];
    long long x = c*bezout[0], y = c*bezout[1];
    if (x < 0 || y > 0)
    {
        long long d = std::max(std::abs(x) / bezout[4], std::abs(y) / bezout[3]);
        x += (d+1)*bezout[4], y -= (d+1)*bezout[3];
    }
    else
    {
        long long d = std::min(x / bezout[4], -y / bezout[3]);
        x -= d*bezout[4], y += d*bezout[3];
    }
    return std::array<int, 2>{static_cast<int>(x), static_cast<int>(y)};
}

int findPushCount(int floors, int start, int goal, int up, int down, int maxUps, int maxDowns)
{
    int pos = start, upCount = 0, downCount = 0;
    while (pos != goal)
    {
        if (pos + up <= floors && upCount < maxUps)
        {
            int k = std::min((floors - pos) / up, maxUps - upCount);
            pos += k*up;
            upCount += k;
        }
        else if (pos >= down && downCount < maxDowns)
        {
            int k = std::min(pos / down, maxDowns - downCount);
            pos -= k*down;
            downCount += k;
        }
        else
            break;
    }
    return pos == goal ? upCount + downCount : k_impossible;
}

int minPushes(int floors, int start, int goal, int up, int down)
{
    int offset = goal - start;
    if (!up && !down)
        return offset ? k_impossible : offset;
    else if (!up)
        return offset > 0 || offset % down ? k_impossible : -offset / down;
    else if (!down)
        return offset < 0 || offset % up ? k_impossible : offset / up;
    else
    {
        auto bezout(egcd(up, down));
        if (offset % bezout[2])
            return k_impossible;
        auto solution(minimalSolution(bezout, offset));
        return findPushCount(floors, start, goal, up, down, solution[0], -solution[1]);
    }
}

int main()
{
    int floors, start, goal, up, down;
    std::cin >> floors >> start >> goal >> up >> down;
    int result = minPushes(floors - 1, start - 1, goal - 1, up, down);
    std::cout << (result == k_impossible ? "use the stairs"s : std::to_string(result));
    return 0;
}
