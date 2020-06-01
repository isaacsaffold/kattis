#include <iostream>
#include <algorithm>
#include <string>

class WorkReductionAgency final
{
    friend bool operator<(const WorkReductionAgency&, const WorkReductionAgency&);
    friend std::ostream& operator<<(std::ostream&, const WorkReductionAgency&);

    private:
        static constexpr int findMinCost(int currentWork, int targetWork, int decrCost, int halveCost)
        {
            int total = 0;
            while (currentWork >= 2 * targetWork)
            {
                if ((currentWork + 1) / 2 * decrCost > halveCost)
                {
                    total += halveCost;
                    currentWork >>= 1;
                }
                else
                    break;
            }
            return total + (currentWork - targetWork) * decrCost;
        }

        std::string m_name;
        int m_minCost;

    public:
        WorkReductionAgency() = default;

        WorkReductionAgency(const std::string& name, int currentWork, int targetWork, int decrCost, int halveCost):
            m_name(name), m_minCost(findMinCost(currentWork, targetWork, decrCost, halveCost)) {}

        const std::string& name() const
        {
            return m_name;
        }

        int minCost() const
        {
            return m_minCost;
        }
};

bool operator<(const WorkReductionAgency& a, const WorkReductionAgency& b)
{
    return a.m_minCost != b.m_minCost ? a.m_minCost < b.m_minCost : a.m_name < b.m_name;
}

std::ostream& operator<<(std::ostream& dest, const WorkReductionAgency& agency)
{
    return dest << agency.m_name << ' ' << agency.m_minCost;
}

int main()
{
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    constexpr int MAX_AGENCIES = 100;
    WorkReductionAgency agencies[MAX_AGENCIES];
    int numCases;
    std::cin >> numCases;
    for (int i = 1; i <= numCases; ++i)
    {
        std::cout << "Case " << i << '\n';
        int currentWork, targetWork, numAgencies;
        std::cin >> currentWork >> targetWork >> numAgencies;
        std::string name;
        std::cin.ignore();
        for (int j = 0; j < numAgencies; ++j)
        {
            std::getline(std::cin, name, ':');
            int decrCost, halveCost;
            std::cin >> decrCost;
            std::cin.ignore();
            std::cin >> halveCost;
            agencies[j] = WorkReductionAgency(name, currentWork, targetWork, decrCost, halveCost);
            std::cin.ignore();
        }
        std::sort(agencies, agencies + numAgencies);
        for (int j = 0; j < numAgencies; ++j)
            std::cout << agencies[j] << '\n';
    }
    return 0;
}
