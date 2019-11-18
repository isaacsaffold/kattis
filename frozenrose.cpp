#include <iostream>
#include <algorithm>
#include <vector>

int minCut(int src, std::vector<int>* adjList, bool* visited)
{
    static constexpr int k_destMask = 0x0000FFFF;

    int currCut = 0;
    for (int n: adjList[src])
    {
        int dest = n & k_destMask, weight = n >> 16;
        if (!visited[dest])
        {
            visited[dest] = true;
            int nextCut = minCut(dest, adjList, visited);
            currCut += (nextCut ? std::min(weight, nextCut) : weight);
        }
    }
    return currCut;
}

int main()
{
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::vector<int> adjList[1001];
    bool visited[1001] = {};
    int order, src;
    while (std::cin >> order && std::cin >> src)
    {
        visited[src] = true;
        for (int i = 1; i < order; ++i)
        {
            int from, to, weight;
            std::cin >> from >> to >> weight;
            adjList[from].push_back(to + (weight << 16));
            adjList[to].push_back(from + (weight << 16));
        }
        std::cout << minCut(src, adjList, visited) << '\n';
        for (int i = 1; i <= order; ++i)
            adjList[i].clear(), visited[i] = false;
    }
    return 0;
}
