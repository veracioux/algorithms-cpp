#include <iostream>

#include "logic/Implicant.h"
#include "logic/McCluskey.h"
#include "programs/McCluskeyProg.h"

using namespace logic;

int main()
{
	std::cout << "\nChoose an algorithm by number: \n"
				 "\t 1 - McCluskey\n"
				 "\nType 'help' to get more information.\n"
			  << "\nYour choice: ";
	std::string command;
	std::getline(std::cin, command);
	if (command == "help")
		std::cout << "";
	else
		switch (command[0])
		{
			case '1':
				mccluskey::main();
				break;
			default:
				std::cout << "Invalid command, try again.";
		}
	system("pause");
	return 0;
}