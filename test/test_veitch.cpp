#include <iostream>

#include "test_veitch.h"

namespace veitch_test
{
	using namespace logic;

	int main()
	{
		test2();
		std::cout << std::endl << std::endl << std::endl;
		test3();
		std::cout << std::endl << std::endl << std::endl;

		test4();
		std::cout << std::endl << std::endl << std::endl;

		test5();
		std::cout << std::endl << std::endl << std::endl;

		return 0;
	}

	void prototype2Contour()
	{
		std::cout <<
				  "+-------+-------+\n"
				  "| +---+ |       |\n"
				  "| | 1 | |   0   |\n"
				  "| |   | |       |\n"
				  "+-|---|-+-------+\n"
				  "| |   | |       |\n"
				  "| | 1 | |   0   |\n"
				  "| +---+ |       |\n"
				  "+-------+-------+\n";
	}

	void prototypeEmpty()
	{
		std::cout <<
				  "+-------+-------+\n"
				  "|       |       |\n"
				  "|   1   |   0   |\n"
				  "|       |       |       |\n"
				  "+-------+-------+\n"
				  "|       |       |\n"
				  "|   1   |   0   |\n"
				  "|       |       |\n"
				  "+-------+-------+\n";
	}

	void test_dimensions()
	{
		VeitchDiagram veitch[5]{VeitchDiagram{1}, VeitchDiagram{2}, VeitchDiagram{3}, VeitchDiagram{4},
								VeitchDiagram{5}};
		for (auto &x : veitch)
			std::cout << x.GetHeight() << " " << x.GetWidth() << std::endl;
	}

	void test1()
	{
		auto vd = VeitchDiagram(4, {{"AB'", 4},
									{"BCD", 4}}, {{"A'B'CD", 4}}, true);
		auto matrix = vd.GetMatrixRepresentation();
		for (int i = 0; i < matrix.size(); ++i)
		{
			for (int j = 0; j < matrix[0].size(); ++j)
				std::cout << matrix[i][j] << " ";
			std::cout << std::endl;
		}
	}

	void prototype2ContourVars()
	{
		std::cout <<
				  "        B   \n"
				  "     -------\n"
				  "    +-------+-------+\n"
				  "  | | +---+ |       |\n"
				  "A | | | 1 | |   0   |\n"
				  "  | | |   | |       |\n"
				  "    +-|---|-+-------+\n"
				  "    | |   | |       |\n"
				  "    | | 1 | |   0   |\n"
				  "    | +---+ |       |\n"
				  "    +-------+-------+\n";
	}

	void test2()
	{
		auto vd = VeitchDiagram(4, {{"AB'", 4},
									{"BCD", 4}}, {{"A'B'CD", 4}}, true);
		std::cout << vd;
	}

	// Tests the corner contour
	void test3()
	{
		VeitchDiagram v(3, {{"ABC'",   3},
							{"AB'C'",  3},
							{"A'BC'",  3},
							{"A'B'C'", 3}}, true);
		std::cout << v;
	}

	void test4()
	{
		VeitchDiagram v(4, {{"ABC'D'",   4},
							{"AB'C'D'",  4},
							{"A'BC'D'",  4},
							{"A'B'C'D'", 4}}, true);
		std::cout << v;
	}

	void test5()
	{
		VeitchDiagram v(2, {{"AB"},
							{"AB'"},
							{"A'B'"}}, true);
		std::cout << v;
	}
}
