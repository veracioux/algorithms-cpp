
#include "McCluskeyProg.h"

namespace mccluskey_prog
{
	bool GetCommaSeparatedValues(logic::DNF &dnf, unsigned char nVariables)
	{
		if (std::cin.peek() == '\n') return true;
		for (;;)
		{
			ullong x;
			if (!(std::cin >> x))
			{
				while (std::cin.peek() == ' ') std::cin.get();
				if (std::cin.peek() != '\n')
				{
					std::cout << "The specified logical function is of illegal format.\n";
					return false;
				}
				std::cin.clear();
			}
			dnf.insert(logic::Implicant(x, nVariables));
			int nCommas = 0;
			while (std::cin.peek() == ' ' || std::cin.peek() == ',')
				if (std::cin.get() == ',' && ++nCommas > 1)
					return false;
			if (nCommas == 0)
				break;
		}
		return true;
	}

	/* Helper function
	 * Extracts a DNF from the stream in literal form
	 */
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
				std::cin >> command;
				dontCare.insert(logic::Implicant(command, nVariables));
			}
			dnf = StringToDNF(function, nVariables);
		} catch (...)
		{
			std::cout << "Argument is of illegal format.\n";
			return false;
		}
		return true;
	}

	// Helper function
	void CinReset()
	{
		std::cin.clear();
		std::cin.ignore(10000, '\n');
	}

	bool ExtractDNFFromMinterms(unsigned char &n, logic::DNF &dnf, logic::DNF &dontCare)
	{
		logic::DNF temp;
		if (!GetCommaSeparatedValues(temp, n))
		{
			std::cout << "The specified logical function is of illegal format.\n";
			return false;
		}
		int nVariables;
		if (!(std::cin >> nVariables) || nVariables <= 0 || nVariables > 64)
		{
			CinReset();
			std::cout << "Legal number of variables must be specified.\n";
			return false;
		}
		else n = nVariables;
		std::transform(temp.begin(), temp.end(), std::inserter(dnf, dnf.begin()), [&](auto &x) {
			return logic::Implicant(x.GetForm(), n);
		});
		if (!GetCommaSeparatedValues(dontCare, n))
		{
			std::cout << "The specified logical function is of illegal format.\n";
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
					std::cin.clear();
					std::cin.ignore(10000, '\n');
					continue;
				}
			}
			else if (command == "minimize")
			{
				logic::DNF dnf, dontCare;
				unsigned char nVariables;
				bool minterms = false;
				while (std::cin.peek() != '\n' && (std::cin >> std::ws, std::cin.peek() == '-'))
				{
					std::cin >> command;
					if (command == "-minterms")
						minterms = true;
					else
					{
						std::cout << "Unknown option.\n";
						continue;
					}
				}
				if (minterms ? ExtractDNFFromMinterms(nVariables, dnf, dontCare) : ExtractDNF(nVariables, dnf, dontCare))
				{
					auto mdnfs = logic::GetMDNF(nVariables, dnf, dontCare);
					std::cout << "The following MDNFs have been found:\n";
					int i = 0;
					for (auto &mdnf : mdnfs)
						std::cout << '\t' << ++i << ". " << logic::ToLiteral(mdnf, nVariables) << std::endl;
				}
				else
					CinReset();
			}
			else if (command == "veitch")
			{
				bool minterms = false, shouldMinimize = true, drawContours = true, drawBorders = true, showAll = false;
				while (std::cin.peek() != '\n' && (std::cin >> std::ws, std::cin.peek() == '-'))
				{
					std::cin >> command;
					if (command == "-minterms")
						minterms = true;
					else if (command == "-nominimize")
						shouldMinimize = false;
					else if (command == "-noborders")
						drawBorders = false;
					else if (command == "-showall")
						showAll = true;
					else
					{
						std::cout << "Unknown option.\n";
						continue;
					}
					//TODO else if (command == "-nocontours")
				}
				logic::DNF dnf, dontCare;
				unsigned char nVariables;
				if (minterms ? ExtractDNFFromMinterms(nVariables, dnf, dontCare) : ExtractDNF(nVariables, dnf, dontCare))
				{
					if (showAll && shouldMinimize)
						for (auto &mdnf : logic::GetMDNF(nVariables, dnf, dontCare))
						{
							logic::VeitchDiagram diagram(nVariables, mdnf, dontCare, false);
							diagram.Print(std::cout, drawBorders) << '\n';
						}
					else
						logic::VeitchDiagram(nVariables, dnf, dontCare, shouldMinimize).Print(std::cout, drawBorders) << '\n';
				}
				else
					CinReset();
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
					<< "Minimize the logical function specified as the argument. Currently supports only disjunctive normal forms.\n" //TODO change in the future
					   "Usage:\n\t minimize %Function% %Number of variables% %[optional] Don't care combinations%\n"
					   "Options:\n"
					   "\t-minterms\tFunction input is in the form of minterm indices.\n"
					   "Examples:\n"
					   "\tminimize ABC+ABC'+A'C 4\n"
					   "\tminimize ABC+AB'C 4 ABC' ABC\n"
					   "\tminimize -minterms 0,1,5,10,13,14 4\n";
		else if (command == "veitch")
			std::cout
					<< "Draw a Veitch diagram based on the logical function specified as the argument. Currently supports only disjunctive normal forms.\n"
					   "The diagram will contain contours based on the specified DNF if 'minimize' option is off, and optimal contours if 'minimize' is on.\n"
					   "Minimization is enabled by default (if -nominimize is not specified). If there are multiple MDNFs of this function, all such diagrams will be shown.\n"
					   "Usage:\n"
					   "\tveitch %Function% %Number of variables% %[optional] Don't care combinations%\n"
					   "Options (specified after the command):\n"
					   "\t-minterms\tFunction input is in the form of minterm indices.\n"
					   "\t-nominimize\tDo not minimize this function.\n"
					   "\t-nocontours\tDraw the diagram without contours. (currently not supported)\n" //TODO add support
					   "\t-noborders\tDo not draw table borders. Can cause for better readability.\n"
					   "\t-showall\tShow Veitch diagrams for all MDNFs of this function (if 'minimize' is on). This is disabled by default.\n"
					   "Examples:\n"
					   "\tveitch ABC+ABC'+A'C 4\n"
					   "\tveitch -nominimize ABC+AB'C 4 ABC' ABC\n"
					   "\tveitch -minterms 0,1,5,10,13,14 4\n";
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