
#include "McCluskeyProg.h"

namespace mccluskey_prog
{
	// Helper function
	bool ExtractDNF(unsigned char &n, logic::DNF &dnf, logic::DNF &dontCare)
	{
		std::string function, command;
		std::cin >> function;
		int nVariables;
		if (!(std::cin >> nVariables) || nVariables <= 0 || nVariables > 64)
		{
			std::cin.clear();
			std::cin.ignore(10000, '\n');
			std::cout << "Legal number of variables must be specified.\n";
			return false;
		}
		else n = nVariables;
		try
		{
			while (std::cin.peek() != '\n')
			{
				//std::cin >> std::ws;
				if (std::cin.peek() != '\n')
				{
					std::cin >> command;
					dontCare.insert(logic::Implicant(command, nVariables));
				}
			}
			dnf = StringToDNF(function, nVariables);
		} catch (...)
		{
			std::cout << "Argument is of illegal format.\n";
			return false;
		}
		return true;
	}

	int main()
	{
		std::cout << "************************************\n"
					 "* McCluskey minimization algorithm *\n"
					 "************************************\n\n"
					 "Type 'help' for a list of commands\n\n";

		for (;;)
		{
			std::cout << ">> ";
			while (std::cin.peek() == '\n')
				std::cin.get();
			std::string command;
			std::cin >> std::ws >> command;
			if (command == "quit")
				break;
			else if (command == "help")
			{
				if (std::cin.peek() != '\n')
					std::cin >> std::ws;
				if (std::cin.peek() == '\n') // Help command with no argument.
				{
					ShowHelp();
					continue;
				}
				std::cin >> command;
				if (!ShowHelp(command))
				{
					std::cout << "Unknown command.\n";
					continue;
				}
			}
			else if (command == "minimize")
			{
				logic::DNF dnf, dontCare;
				unsigned char nVariables;
				if (ExtractDNF(nVariables, dnf, dontCare))
				{
					auto mdnfs = logic::GetMDNF(nVariables, dnf, dontCare);
					std::cout << "The following MDNFs have been found:\n";
					int i = 0;
					for (auto &mdnf : mdnfs)
						std::cout << '\t' << ++i << ". " << logic::ToLiteral(mdnf, nVariables) << std::endl;
				}
			}
			else if (command == "veitch")
			{
				bool shouldMinimize = true, drawContours = true, drawBorders = true;
				while (std::cin.peek() != '\n' && (std::cin >> std::ws, std::cin.peek() == '-'))
				{
					std::cin >> command;
					if (command == "-nominimize")
						shouldMinimize = false;
					else if (command == "-noborders")
						drawBorders = false;
					else
					{
						std::cout << "Unknown option.\n";
						continue;
					}
					//TODO else if (command == "nocontours")
				}
				logic::DNF dnf, dontCare;
				unsigned char nVariables;
				if (ExtractDNF(nVariables, dnf, dontCare))
				{
					logic::VeitchDiagram diagram(nVariables, dnf, dontCare, shouldMinimize);
					diagram.Print(std::cout, drawBorders) << '\n';
				}
			}
			else
				std::cout << "Unknown command.\n";
		}
		std::cout << "Quitting McCluskey.\n";
		return 0;
	}

	bool ShowHelp(const std::string &command)
	{
		if (command == "minimize")
			std::cout
					<< "Minimize the logical function specified as the argument. Currently supports only disjunctive normal forms\n" //TODO change in future
					   "Proper usage:\n\t minimimize %Function% %Number of variables% %[optional] Don't care combinations%\n"
					   "Examples:\n"
					   "\t minimize ABC+ABC'+A'C 4\n"
					   "\t minimize ABC+AB'C 4 ABC' ABC\n";
		else if (command == "veitch")
			std::cout
					<< "Draw a Veitch diagram based on the logical function specified as the argument. Currently supports only disjunctive normal forms.\n"
					   "The diagram will contain contours based on the specified DNF if 'minimize' option is off, and optimal contours if 'minimize' is on.\n"
					   "Minimization is enabled by default (if -nominimize is not specified). If there are multiple MDNFs of this function, all such diagrams will be shown.\n"
					   "Proper usage:\n"
					   "\tveitch %Function% %Number of variables% %[optional] Don't care combinations%\n"
					   "Options (specified after the command):\n"
					   "\t-nominimize\tDo not minimize this function.\n"
					   "\t-nocontours\tDraw the diagram without contours. (currently not supported)\n" //TODO add support
					   "\t-noborders\tDo not draw table borders. Can cause for better readability.\n"
					   "Examples:\n"
					   "\t veitch ABC+ABC'+A'C 4\n"
					   "\t veitch -nominimize ABC+AB'C 4 ABC' ABC\n";
		else
			return false;
		return true;
	}

	void ShowHelp()
	{
		std::cout << "Available commands:\n"
					 "\tminimize\n"
					 "\tveitch\n"
					 "\tquit\tQuits the current algorithm.\n"
					 "Type 'help %command%' to get command-specific help.\n";
	}

	logic::DNF StringToDNF(const std::string &s, unsigned char n)
	{
		logic::DNF dnf;
		int j = 0;
		for (int i = 0; i < s.size(); ++i)
			if (s[i] == '+')
			{
				std::string sub(i - j, ' ');
				for (int k = j; k < i; ++k)
					sub[k - j] = s[k];
				dnf.insert(logic::Implicant(sub, n));
				j = i + 1;
			}
		dnf.insert(logic::Implicant(s.substr(j), n));
		return dnf;
	}
}