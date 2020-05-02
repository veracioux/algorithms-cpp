#include "program.h"

arglist read_line()
{
	std::cin.clear();
	std::vector<std::string> result;
	// Scan the line char by char
	while (std::cin.peek() != '\n')
	{
		// Remove any whitespace
		while (std::isspace(std::cin.peek()) && std::cin.peek() != '\n')
			std::cin.get();
		std::string in;
		// Text enclosed in double quotes is treated as a single word
		if (std::cin.peek() == '"')
		{
			// The quote-sign is not included
			std::cin.get();
			// Look for end-quote
			while (std::cin.peek() != '"' && std::cin.peek() != '\n')
				in += (char) std::cin.get();
			if (std::cin.peek() == '\n')
				throw std::logic_error("Unterminated quote found");
			// Eat the end-quote
			std::cin.get();
		}
		else if (std::cin.peek() == '\n')
			return std::cin.get(), result;
		else
			std::cin >> in;
		result.push_back(in);
	}
	std::cin.get();
	return result;
}

void unrecognized_opt_cmd(const std::string &opt)
{
	std::cerr << opt << ": unrecognized " << (!opt.empty() && opt[0] == '-' ? "option" : "command.\n");
}
