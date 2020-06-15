#ifndef BABYNAMES_OBJECT_POOL_HPP_692020
#define BABYNAMES_OBJECT_POOL_HPP_692020

#include <vector>
#include <stack>
#include <deque>
#include <numeric>
#include <utility>

template <typename T, typename SizeType = unsigned int>
class ObjectPool final
{
    private:
        static std::deque<SizeType> indexContainer(SizeType size);

        std::vector<T> m_objects;
        std::stack<SizeType> m_unusedIndices;

    public:
        using object_t = T;
        using size_t = SizeType;

        ObjectPool() = default;
        explicit ObjectPool(SizeType initialSize);

        SizeType size() const;
        SizeType numUnused() const;

        T& operator[](SizeType index);
        const T& operator[](SizeType index) const;

        template <typename... Args>
        SizeType obtain(Args&&... args);

        void release(SizeType index);
};

template <typename T, typename SizeType>
std::deque<SizeType> ObjectPool<T, SizeType>::indexContainer(SizeType size)
{
    std::deque<SizeType> container(size);
    std::iota(container.begin(), container.end(), 0);
    return container;
}

template <typename T, typename SizeType>
ObjectPool<T, SizeType>::ObjectPool(SizeType initialSize):
    m_objects(initialSize), m_unusedIndices(indexContainer(initialSize)) {}

template <typename T, typename SizeType>
SizeType ObjectPool<T, SizeType>::size() const
{
    return m_objects.size();
}

template <typename T, typename SizeType>
SizeType ObjectPool<T, SizeType>::numUnused() const
{
    return m_unusedIndices.size();
}

template <typename T, typename SizeType>
T& ObjectPool<T, SizeType>::operator[](SizeType index)
{
    return m_objects[index - 1];
}

template <typename T, typename SizeType>
const T& ObjectPool<T, SizeType>::operator[](SizeType index) const
{
    return m_objects[index - 1];
}

template <typename T, typename SizeType>
template <typename... Args>
SizeType ObjectPool<T, SizeType>::obtain(Args&&... args)
{
    if (m_unusedIndices.empty())
    {
        m_objects.emplace_back(std::forward<Args>(args)...);
        return m_objects.size();
    }
    else
    {
        SizeType index = m_unusedIndices.top();
        m_objects[index] = T(std::forward<Args>(args)...);
        m_unusedIndices.pop();
        return index + 1;
    }
}

template <typename T, typename SizeType>
void ObjectPool<T, SizeType>::release(SizeType index)
{
    m_unusedIndices.push(index - 1);
}

#endif

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

class NamePrefixTree final
{
    public:
        enum class Gender: unsigned char
        {
            MALE = 1,
            FEMALE = 2,
            BOTH = 3
        };

    private:
        struct Node
        {
            char letter;
            unsigned char gender;
            std::vector<int> children;
            int numDescendants[3];
        };

        static int numDescendantsWithGender(const Node& node, unsigned char gender);

        ObjectPool<Node> m_pool;
        int m_rootID;

        std::vector<int>::const_iterator findNodeWithLetter(const std::vector<int>& ids, char letter) const;
        unsigned char implAdd(const std::string& name, unsigned char gender, int id, int index);
        unsigned char implRemove(const std::string& name, int id, int index);
        int numLessThan(const std::string& str, unsigned char gender) const;

    public:
        explicit NamePrefixTree(int initialCapacity);

        void add(const std::string& name, Gender gender);
        void remove(const std::string& name);
        int numInInterval(const std::string& start, const std::string& end, Gender gender) const;
};

int NamePrefixTree::numDescendantsWithGender(const Node& node, unsigned char gender)
{
    int n = node.numDescendants[2];
    if (gender & 1)
        n += node.numDescendants[0];
    if (gender & 2)
        n += node.numDescendants[1];
    return n;
}

NamePrefixTree::NamePrefixTree(int initialCapacity = 0):
    m_pool(initialCapacity), m_rootID(m_pool.obtain(Node{'\0', 0})) {}

std::vector<int>::const_iterator NamePrefixTree::findNodeWithLetter(const std::vector<int>& ids, char letter) const
{
    return std::find_if(ids.begin(), ids.end(), [&](int n){return m_pool[n].letter == letter;});
}

unsigned char NamePrefixTree::implAdd(const std::string& name, unsigned char gender, int id, int index)
{
    Node* node = &m_pool[id];
    if (index == name.size())
    {
        unsigned char action = node->gender << 2;
        node->gender |= gender;
        return action | node->gender;
    }
    else
    {
        int childID;
        std::vector<int>::const_iterator childIter(findNodeWithLetter(node->children, name[index]));
        if (childIter == node->children.end())
        {
            childID = m_pool.obtain(Node{name[index], 0});
            node = &m_pool[id];
            node->children.push_back(childID);
        }
        else
            childID = *childIter;
        unsigned char action = implAdd(name, gender, childID, index + 1);
        node = &m_pool[id];
        switch (action)
        {
            case 1:
            case 2:
                ++node->numDescendants[action - 1];
                break;
            case 7:
                --node->numDescendants[0];
                ++node->numDescendants[2];
                break;
            case 11:
                --node->numDescendants[1];
                ++node->numDescendants[2];
                break;
        }
        return action;
    }
}

unsigned char NamePrefixTree::implRemove(const std::string& name, int id, int index)
{
    Node& parent = m_pool[id];
    std::vector<int>::const_iterator childIter(findNodeWithLetter(parent.children, name[index]));
    if (childIter == parent.children.end())
        return 0;
    unsigned char gender;
    Node& child = m_pool[*childIter];
    if (index == name.size() - 1)
    {
        gender = child.gender;
        child.gender = 0;
    }
    else if (!(gender = implRemove(name, *childIter, index + 1)))
        return 0;
    --parent.numDescendants[gender - 1];
    if (!child.gender && !child.numDescendants[0] && !child.numDescendants[1] && !child.numDescendants[2])
    {
        m_pool.release(*childIter);
        parent.children.erase(childIter);
    }
    return gender;
}

int NamePrefixTree::numLessThan(const std::string& str, unsigned char gender) const
{
    int total = 0;
    int parentID = m_rootID;
    for (int i = 0, n = str.size() - 1; i < n; ++i)
    {
        int nextID = 0;
        for (int childID: m_pool[parentID].children)
        {
            const Node& child = m_pool[childID];
            if (child.letter < str[i])
                total += numDescendantsWithGender(child, gender) + !!(child.gender & gender);
            else if (child.letter == str[i])
            {
                total += !!(child.gender & gender);
                nextID = childID;
            }
        }
        if (!nextID)
            return total;
        parentID = nextID;
    }
    for (int childID: m_pool[parentID].children)
    {
        const Node& child = m_pool[childID];
        if (child.letter < str.back())
            total += numDescendantsWithGender(child, gender) + !!(child.gender & gender);
    }
    return total;
}

void NamePrefixTree::add(const std::string& name, Gender gender)
{
    implAdd(name, static_cast<unsigned char>(gender), m_rootID, 0);
}

void NamePrefixTree::remove(const std::string& name)
{
    implRemove(name, m_rootID, 0);
}

int NamePrefixTree::numInInterval(const std::string& start, const std::string& end, Gender gender) const
{
    unsigned char g = static_cast<unsigned char>(gender);
    return numLessThan(end, g) - numLessThan(start, g);
}

int main()
{
    static constexpr NamePrefixTree::Gender genders[3]
    {
        NamePrefixTree::Gender::BOTH,
        NamePrefixTree::Gender::MALE,
        NamePrefixTree::Gender::FEMALE
    };
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    NamePrefixTree names;
    std::string s1, s2;
    while (true)
    {
        int command;
        std::cin >> command;
        std::cin.ignore();
        switch (int n; command)
        {
            case 0:
                return 0;
            case 1:
                std::cin >> s1 >> n;
                names.add(s1, genders[n]);
                break;
            case 2:
                std::cin >> s1;
                names.remove(s1);
                break;
            case 3:
                std::getline(std::cin, s1, ' ');
                std::cin >> s2 >> n;
                std::cout << names.numInInterval(s1, s2, genders[n]) << '\n';
                break;
        }
    }
}
