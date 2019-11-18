#include <iostream>
#include <algorithm>
#include <utility>

template <typename NumType>
struct Point
{
    NumType x, y;
};

template <typename T, typename U>
unsigned char findQuadrant(const Point<T>& point, const Point<U>& origin)
{
    bool inTopHalf = point.y > origin.y;
    return point.x < origin.x ? inTopHalf : 3 - inTopHalf;
}

int bitCount(int n)
{
    int count = 1;
    while (n >>= 1)
        ++count;
    return count;
}

unsigned long long hilbertOrdinal(const Point<int>& point, int gridLen)
{
    long double quadLen = gridLen / 2.0;
    Point<long double> origin{quadLen, quadLen}, v{point.x, point.y};
    unsigned long long ordinal = 0;
    for (int i = 2 * (bitCount(gridLen) - 1); i >= 0; i -= 2)
    {
        unsigned char quadrant = findQuadrant(v, origin);
        ordinal |= static_cast<unsigned long long>(quadrant) << i;
        quadLen /= 2;
        long double r;
        switch (quadrant)
        {
            case 0:
                origin.x -= quadLen, origin.y -= quadLen;
                r = origin.x - origin.y;
                v.x -= r, v.y += r;
                std::swap(v.x, v.y);
                break;
            case 1:
                origin.x -= quadLen, origin.y += quadLen;
                break;
            case 2:
                origin.x += quadLen, origin.y += quadLen;
                break;
            case 3:
                origin.x += quadLen, origin.y -= quadLen;
                r = origin.x + origin.y;
                v.x = r - v.x, v.y = r - v.y;
                std::swap(v.x, v.y);
                break;
        }
    }
    return ordinal;
}

struct Mapping
{
    unsigned long long ordinal;
    int index;
};

int main()
{
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int pointCount, gridLen;
    std::cin >> pointCount >> gridLen;
    Point<int>* points = new Point<int>[pointCount];
    Mapping* hilbertMapping = new Mapping[pointCount]();
    for (int i = 0; i < pointCount; ++i)
    {
        std::cin >> points[i].x >> points[i].y;
        hilbertMapping[i].ordinal = hilbertOrdinal(points[i], gridLen);
        hilbertMapping[i].index = i;
    }
    std::sort(hilbertMapping, hilbertMapping + pointCount,
        [](const auto& a, const auto& b){return a.ordinal < b.ordinal;});
    for (int i = 0; i < pointCount; ++i)
    {
        Point<int>& point = points[hilbertMapping[i].index];
        std::cout << point.x << ' ' << point.y << '\n';
    }
    delete[] points, delete[] hilbertMapping;
    return 0;
}
