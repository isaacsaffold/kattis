#include <iostream>
#include <cstring>
#include <array>
#include <vector>

class Graph final
{
    private:
        static constexpr int BASE = 7;
        static constexpr int MODULUS = 1'000'000'007;

        bool* m_data;
        bool** m_rows;
        bool* m_wasComplementWhenAdded;
        int m_order;
        bool m_transposed = false;
        bool m_isComplement = false;
        mutable std::vector<int> m_powers;

        bool& transformed(int, int);
        const bool& transformed(int, int) const;

    public:
        struct VertexInfo
        {
            int outdegree;
            int hash;
        };

        enum class Command
        {
            ADD_VERTEX,
            ADD_EDGE,
            REMOVE_EDGE,
            ISOLATE_VERTEX,
            TRANSPOSE,
            MAKE_COMPLEMENT
        };

        struct Action
        {
            Command type;
            int args[2];
        };

        template <typename Iterator>
        Graph(int, int, Iterator, Iterator);

        ~Graph();

        VertexInfo vertexInfo(int) const;

        void addVertex();
        void addEdge(int, int);
        void removeEdge(int, int);
        void isolateVertex(int);
        void transpose();
        void makeComplement();

        void performAction(const Action&);
};

bool& Graph::transformed(int row, int col)
{
    return const_cast<bool&>(static_cast<const Graph*>(this)->transformed(row, col));
}

const bool& Graph::transformed(int row, int col) const
{
    return m_transposed ? m_rows[col][row] : m_rows[row][col];
}

template <typename Iterator>
Graph::Graph(int initialOrder, int maxOrder, Iterator edgesBegin, Iterator edgesEnd):
    m_data(new bool[maxOrder * maxOrder]()), m_rows(new bool*[maxOrder]),
    m_wasComplementWhenAdded(new bool[maxOrder]()), m_order(initialOrder)
{
    bool* ptr = m_data;
    for (int i = 0; i < maxOrder; ++i)
    {
        m_rows[i] = ptr;
        ptr += maxOrder;
    }
    for (; edgesBegin != edgesEnd; ++edgesBegin)
    {
        const auto& [from, to] = *edgesBegin;
        m_rows[from][to] = true;
    }
}

Graph::~Graph()
{
    delete[] m_data, delete[] m_rows, delete[] m_wasComplementWhenAdded;
}

Graph::VertexInfo Graph::vertexInfo(int vertex) const
{
    if (m_powers.empty())
    {
        m_powers.resize(m_order);
        m_powers[0] = 1;
        for (int i = 1; i < m_order; ++i)
            m_powers[i] = BASE * static_cast<long long>(m_powers[i - 1]) % MODULUS;
    }
    VertexInfo vinfo{};
    long long i = 0;
    for (; i < vertex; ++i)
    {
        if (transformed(vertex, i) ^ m_isComplement ^ m_wasComplementWhenAdded[vertex])
            vinfo.hash = (static_cast<long long>(vinfo.hash) + i*m_powers[vinfo.outdegree++]) % MODULUS;
    }
    while (++i < m_order)
    {
        if (transformed(vertex, i) ^ m_isComplement ^ m_wasComplementWhenAdded[i])
            vinfo.hash = (static_cast<long long>(vinfo.hash) + i*m_powers[vinfo.outdegree++]) % MODULUS;
    }
    return vinfo;
}

void Graph::addVertex()
{
    m_wasComplementWhenAdded[m_order++] = m_isComplement;
}

void Graph::addEdge(int from, int to)
{
    transformed(from, to) ^= true;
}

void Graph::removeEdge(int from, int to)
{
    transformed(from, to) ^= true;
}

void Graph::isolateVertex(int vertex)
{
    std::memset(m_rows[vertex], m_isComplement ^ m_wasComplementWhenAdded[vertex], vertex * sizeof(bool));
    int i = 0;
    for (; i < vertex; ++i)
        m_rows[i][vertex] = m_isComplement ^ m_wasComplementWhenAdded[vertex];
    while (++i < m_order)
    {
        m_rows[vertex][i] = m_isComplement ^ m_wasComplementWhenAdded[i];
        m_rows[i][vertex] = m_isComplement ^ m_wasComplementWhenAdded[i];
    }
}

void Graph::transpose()
{
    m_transposed ^= true;
}

void Graph::makeComplement()
{
    m_isComplement ^= true;
}

void Graph::performAction(const Graph::Action& action)
{
    switch (action.type)
    {
        case Command::ADD_VERTEX:
            addVertex();
            break;
        case Command::ADD_EDGE:
            addEdge(action.args[0], action.args[1]);
            break;
        case Command::REMOVE_EDGE:
            removeEdge(action.args[0], action.args[1]);
            break;
        case Command::ISOLATE_VERTEX:
            isolateVertex(action.args[0]);
            break;
        case Command::TRANSPOSE:
            transpose();
            break;
        case Command::MAKE_COMPLEMENT:
            makeComplement();
            break;
    }
}

int main()
{
    int initialOrder, initialSize, numActions;
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cin >> initialOrder >> initialSize >> numActions;
    std::vector<std::array<int, 2>> edges(initialSize);
    for (int i = 0; i < initialSize; ++i)
        std::cin >> edges[i][0] >> edges[i][1];
    std::vector<Graph::Action> actions(numActions);
    int maxOrder = initialOrder;
    for (int i = 0; i < numActions; ++i)
    {
        int n;
        std::cin >> n;
        switch (n)
        {
            case 1:
                actions[i].type = Graph::Command::ADD_VERTEX;
                ++maxOrder;
                break;
            case 2:
                actions[i].type = Graph::Command::ADD_EDGE;
                std::cin >> actions[i].args[0] >> actions[i].args[1];
                break;
            case 3:
                actions[i].type = Graph::Command::ISOLATE_VERTEX;
                std::cin >> actions[i].args[0];
                break;
            case 4:
                actions[i].type = Graph::Command::REMOVE_EDGE;
                std::cin >> actions[i].args[0] >> actions[i].args[1];
                break;
            case 5:
                actions[i].type = Graph::Command::TRANSPOSE;
                break;
            case 6:
                actions[i].type = Graph::Command::MAKE_COMPLEMENT;
                break;
        }
    }
    Graph graph(initialOrder, maxOrder, edges.cbegin(), edges.cend());
    for (const Graph::Action& action: actions)
        graph.performAction(action);
    std::cout << maxOrder << '\n';
    for (int i = 0; i < maxOrder; ++i)
    {
        Graph::VertexInfo vinfo(graph.vertexInfo(i));
        std::cout << vinfo.outdegree << ' ' << vinfo.hash << '\n';
    }
    return 0;
}
