#include <iostream>
#include "Logic.h"

int main()
{
	logic::Implicant<4> a(1, 1);
	std::cout << a << std::endl;
	return 0;
}