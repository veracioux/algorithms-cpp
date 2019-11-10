#include <iostream>
#include "Logic.h"

using namespace logic;

int main()
{

	std::set<Implicant> mt1{{"ABC'"},
							{"ABC"},
							{"AB'C"},
							{"A'BC"},
							{"A'B'C"}}, dc1{};
	SetNumberOfVariables(4);
	std::set<Implicant> mt2{0, 1, 2, 3, 4, 5, 6, 10, 14}, dc2{7, 11, 15};
	std::set<Implicant> mt3{{"ABC"},
							{"AB'C"},
							{"A'BC"},
							{"A'B'C"}}, dc3{{"AB'C'"}};
	std::set<Implicant> mt4{{"A'BCD"},
							{"AB'CD"},
							{"ABCD"}}, dc4{{"A'B'CD"}};
	std::set<Implicant> mt5{{"A'B'CD'"},
							{"A'BCD'"},
							{"A'BCD"},
							{"ABCD"},
							{"AB'CD"}}, dc5{};
	//std::cout << (Implicant("A'BC'") | Implicant{"ABC'"}).ToLiteral(3);

	std::cout << "First function:\n";
	for (auto x : GetMDNF(mt5, dc5))
		std::cout << ToLiteral(x, 4) << std::endl;
	std::cout << "Fifth function:\n";
	for (auto x : GetMDNF(mt4, dc4))
		std::cout << ToLiteral(x, 4) << std::endl;
	return 0;
}