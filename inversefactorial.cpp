#include <iostream>
#include <cmath>
#include <string>
#include <cstdlib>

int invFactorial(const std::string& digits)
{
	if (digits.size() <= 3)
	{
		int n = std::stoi(digits);
		switch (n)
		{
			case 1:
				return 1;
			case 2:
				return 2;
			case 6:
				return 3;
			case 24:
				return 4;
			case 120:
				return 5;
			case 720:
				return 6;
			default:
				std::cerr << n << " is not a factorial.";
				std::exit(1);
		}
	}
	else
	{
		double sum = 0.0;
		int k = 1;
		while (sum < digits.size())
			sum += std::log10(++k);
		return k - 1;
	}
}

int main()
{
	std::ios::sync_with_stdio(false);
	std::cin.tie(nullptr);
	
	std::string digits;
	std::getline(std::cin, digits);
	std::cout << invFactorial(digits);
	return 0;
}