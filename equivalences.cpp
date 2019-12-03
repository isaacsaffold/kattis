#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <stack>
#include <numeric>

using adjlist_t = std::unordered_map<int, std::vector<int>>;

class DisjointSets
{
    private:
        int* m_sets;
        unsigned char* m_heights;
        int m_size;

    public:
        explicit DisjointSets(int len):
            m_sets(new int[len]), m_heights(new unsigned char[len]()), m_size(len) {std::iota(m_sets, m_sets + len, 0);}

        ~DisjointSets() {delete[] m_sets, delete[] m_heights;}

        int size() const {return m_size;}

        int findRoot(int index)
        {
            int height = 0;
            while (index != m_sets[index])
            {
                index = m_sets[index], m_sets[index] = m_sets[m_sets[index]];
                ++height;
            }
            m_heights[index] -= height >> 1;
            return index;
        }

        bool joinSets(int indexA, int indexB)
        {
            int rootA = findRoot(indexA), rootB = findRoot(indexB);
            if (rootA == rootB)
                return false;
            else
            {
                auto byHeight(std::minmax(rootA, rootB, [=](int a, int b){return m_heights[a] < m_heights[b];}));
                m_sets[byHeight.first] = m_sets[byHeight.second];
                if (m_heights[byHeight.first] == m_heights[byHeight.second])
                    ++m_heights[byHeight.second];
                --m_size;
                return true;
            }
        }
};

adjlist_t adjList(int edgeCount, int vertexCount)
{
    adjlist_t edges(vertexCount);
    int k, v;
    for (int i = 0; i < edgeCount; ++i)
    {
        std::cin >> k >> v;
        auto query(edges.find(k));
        if (query != edges.end())
            query->second.push_back(v);
        else
            edges.emplace(k, std::vector<int>{v});
    }
    return edges;
}

class Condenser
{
    private:
        struct VertexInfo
        {
            int stackIndex;
            int rootIndex;
            bool onStack;
        };

        const adjlist_t& m_edges;
        DisjointSets& m_components;
        std::unordered_map<int, VertexInfo> m_info;
        std::stack<int> m_stack;

        void condense(int v)
        {
            m_info.emplace(v, VertexInfo{m_stack.size(), m_stack.size(), true});
            m_stack.push(v);
            VertexInfo& vinfo = m_info.at(v);
            auto childQuery(m_edges.find(v));
            for (int child: childQuery != m_edges.end() ? childQuery->second : std::initializer_list<int>())
            {
                auto infoQuery(m_info.find(child));
                if (infoQuery == m_info.end())
                {
                    condense(child);
                    vinfo.rootIndex = std::min(m_info.at(child).rootIndex, vinfo.rootIndex);
                }
                else if (infoQuery->second.onStack)
                    vinfo.rootIndex = std::min(infoQuery->second.stackIndex, vinfo.rootIndex);
            }
            if (vinfo.stackIndex == vinfo.rootIndex)
            {
                for (int i = m_stack.size(); --i >= vinfo.stackIndex;)
                {
                    m_components.joinSets(m_stack.top(), v);
                    m_info.at(m_stack.top()).onStack = false;
                    m_stack.pop();
                }
            }
        }

    public:
        Condenser(const adjlist_t& edges, DisjointSets& components, int vertexCount):
            m_edges(edges), m_components(components), m_info(vertexCount) {}

        void operator()()
        {
            for (const auto& elem: m_edges)
            {
                if (!m_info.count(elem.first))
                    condense(elem.first);
            }
        }
};

int edgesRequired(int edgeCount, int vertexCount)
{
    constexpr unsigned char k_hasFrom = 1, k_hasTo = 2;
    adjlist_t edges(adjList(edgeCount, vertexCount));
    DisjointSets components(vertexCount);
    Condenser(edges, components, vertexCount)();
    int componentCount = components.size() - 1;
    if (componentCount < 2)
        return 0;
    int fromsRequired = componentCount, tosRequired = componentCount;
    unsigned char* fromTo = new unsigned char[vertexCount]();
    for (const auto& elem: edges)
    {
        int orig = components.findRoot(elem.first);
        for (int dest: elem.second)
        {
            dest = components.findRoot(dest);
            if (orig != dest)
            {
                if (!(fromTo[orig] & k_hasFrom))
                {
                    fromTo[orig] |= k_hasFrom;
                    --fromsRequired;
                }
                if (!(fromTo[dest] & k_hasTo))
                {
                    fromTo[dest] |= k_hasTo;
                    --tosRequired;
                }
            }
        }
    }
    delete[] fromTo;
    return std::max(fromsRequired, tosRequired);
}

int main()
{
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int caseCount;
    std::cin >> caseCount;
    for (int i = 0; i < caseCount; ++i)
    {
        int statementCount, implicationCount;
        std::cin >> statementCount >> implicationCount;
        std::cout << edgesRequired(implicationCount, statementCount + 1) << '\n';
    }
    return 0;
}
