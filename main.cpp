#include <iostream>

#include "logic/Implicant.h"
#include "logic/McCluskey.h"
#include "programs/McCluskeyProg.h"
#include "test/VeitchDiagramTest.h"

using namespace logic;

int main()
{
	for (;;)
	{
		std::cout << "\nChoose an algorithm by number: \n"
					 "\t 1 - McCluskey\n"
					 "\nType 'help' to get more information.\n";
		SKIP:
		std::cout << "\n>> ";
		std::string command;
		while (std::cin.peek() == '\n')
			std::cin.get();
		std::getline(std::cin, command);
		if (command == "help") // TODO implement
		{
			std::cout << "When inside an algorithm, type 'help' to see a list of commands.\n";
			goto SKIP;
		}
		else if (command == "quit")
			break;
		else
			switch (command[0])
			{
				case '1':
					mccluskey_prog::main();
					break;
				default:
					std::cout << "Invalid command, try again.";
			}
	}
	return 0;
}