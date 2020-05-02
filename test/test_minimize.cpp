#include "test_minimize.h"
#include <stdexcept>

void test1()
{
	PrintMDNF(4, {4, 5, 6, 9, 11, 13});
	// Expected: BC'D+AB'D+A'BD'
}

void test2()
{
	PrintMDNF(3, {"ABC'",
				  "ABC",
				  "AB'C",
				  "A'BC",
				  "A'B'C"});
	// Expected: AB+C
}

void test3()
{
	PrintMDNF(4, {0ULL, 1, 2, 3, 4, 5, 6, 10, 14}, {7, 11, 15});
}

void test4()
{
	PrintMDNF(3, {{"ABC"},
				  {"AB'C"},
				  {"A'BC"},
				  {"A'B'C"}}, {{"AB'C'"}});

}

void test5()
{
	PrintMDNF(4, {{"A'BCD"},
				  {"AB'CD"},
				  {"ABCD"}}, {{"A'B'CD"}});
}

void test6()
{
	PrintMDNF(4, {{"A'BCD"},
				  {"AB'CD"},
				  {"ABCD"}}, {{"A'B'CD"}});

}

void test7()
{
	PrintMDNF(4, {{"A'B'CD'"},
				  {"A'BCD'"},
				  {"A'BCD"},
				  {"ABCD"},
				  {"AB'CD"}});

}

void test8()
{
	PrintMDNF(6, {{"A'B'C'D'E'F'"},
				  {"A'B'C'D'EF'"},
				  {"AB'C'D'E'F'"},
				  {"AB'C'D'E'F'"},
				  {"AB'C'D'EF'"},
				  {"ABC'D'E'F'"},
				  {"ABC'D'EF'"}});

}

void test9()
{
	PrintMDNF(4, {0ULL,
				  2,
				  3,
				  5,
				  10,
				  11,
				  12,
				  14,
				  15});

}

void test10()
{
	PrintMDNF(4, {0ULL,
				  1,
				  2,
				  5,
				  6,
				  10,
				  13});

}

void test11()
{
	PrintMDNF(4, {3,
				  4,
				  7,
				  8,
				  9,
				  11,
				  12,
				  14,
				  15});

}

void test12()
{
	PrintMDNF(5, {{"A'CE",    5},
				  {"ABDE'",   5},
				  {"AB'C'E",  5},
				  {"ACE",     5},
				  {"ABDE",    5},
				  {"AB'D'E'", 5}});
	// Expected: AB'D'+ABD+AB'E+CE
	//			 AB'D'+ABD+ADE+CE
}

void testExtrapolate1()
{
	Implicant i = {"D"};
	std::cout << i.ToLiteral(4);
	DNF dnf;
	ExtrapolateMintermsAndAdd(4, i, dnf);
}
