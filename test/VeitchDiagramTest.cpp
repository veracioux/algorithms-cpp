#include <iostream>

#include "VeitchDiagramTest.h"

namespace veitch_test
{
	using namespace logic;

	int main()
	{
		test1();
		return 0;
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
									{"BCD", 4}});
		auto matrix = vd.GetMatrixRepresentation();
		for (int i = 0; i < matrix.size(); ++i)
		{
			for (int j = 0; j < matrix[0].size(); ++j)
				std::cout << matrix[i][j] << " ";
			std::cout << std::endl;
		}
	}
}
