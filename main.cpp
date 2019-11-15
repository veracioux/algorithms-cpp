#include <iostream>

#include "logic/Implicant.h"
#include "logic/McCluskey.h"
#include "programs/McCluskeyProg.h"
#include "test/VeitchDiagramTest.h"

using namespace logic;

int main()
{
	veitch_test::main();
	std::cout << "\nChoose an algorithm by number: \n"
				 "\t 1 - McCluskey\n"
				 "\nType 'help' to get more information.\n"
			  << "\nYour choice: ";
	std::string command;
	std::getline(std::cin, command);
	if (command == "help") // TODO implement
		std::cout << "";
	else
		switch (command[0])
		{
			case '1':
				mccluskey_prog::main();
				break;
			default:
				std::cout << "Invalid command, try again.";
		}
	system("pause");
	return 0;
}