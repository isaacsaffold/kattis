#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>
#include <numeric>

int minRequiredIndex(const char* const* matrix, const std::vector<short>& group, int row)
{
    std::vector<short> newGroups[26];
    for (short col: group)
        newGroups[matrix[row][col] - 'a'].push_back(col);
    int currMin = row;
    for (int i = 0; i < 26; ++i)
    {
        if (newGroups[i].size() > 1)
            currMin = std::min(currMin, minRequiredIndex(matrix, newGroups[i], row - 1));
    }
    return currMin;
}

int main()
{
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int rowCount, colCount;
    std::cin >> rowCount >> colCount;
    char** matrix = new char*[rowCount];
    for (int i = 0; i < rowCount; ++i)
    {
        matrix[i] = new char[colCount];
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cin.read(matrix[i], colCount);
    }
    std::vector<short> group(colCount);
    std::iota(group.begin(), group.end(), 0);
    std::cout << minRequiredIndex(matrix, group, rowCount - 1);
    for (int i = 0; i < rowCount; ++i)
        delete[] matrix[i];
    delete[] matrix;
    return 0;
}
