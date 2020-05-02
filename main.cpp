#include <iostream>
#include <map>
#include <algorithm>


#include "test/test_minimize.h"
#include "programs/program.h"
#include "programs/minimize.h"
#include "programs/veitch.h"

using namespace logic;

std::map<std::string, int (*)(const arglist_iter &, const arglist_iter &)> commands = {
		{"minimize", minimize::minimize},
		{"veitch",   veitch::veitch}
};

/**
 * Execute a command, or print a message if it does not exist.
 * @param it_arg Iterator to the first argument in the list.
 * @param end - End of the argument list
 * @return Status code of the executed command.
 */
int execute_command(const arglist_iter &it_arg, const arglist_iter &end)
{
	// Can the argument be found among the defined commands?
	if (commands.find(*it_arg) != commands.end())
	{ // Yes: execute it
		return commands[*it_arg](it_arg + 1, end);
	}
	else
	{ // No: print an error message
		unrecognized_opt_cmd(*it_arg);
		return -1;
	}
}

int interactive()
{
	arglist args;
	for (;;)
	{
		std::cout << "> ";
		args = read_line();
		if (args.empty()) continue;

		if (commands.find(args[0]) != commands.end())
		{ // Valid command
			commands[args[0]](args.begin() + 1, args.end());
		}
		else if (args[0] == "exit")
		{
			if (args.size() > 1)
				std::cerr << "This command does not take any arguments.\n";
			else return 0;
		}
		else if (args[0] == "clear")
		{
			if (args.size() > 1)
				std::cerr << "This command does not take any arguments.\n";
			else
				std::printf("\033c"); // LF character
		}
		else
			std::cerr << args[0] << ": unrecognized command.\n";
	}
}

int main(int argc, char *argv[])
{
	if (argc < 2)
		return std::cerr << "No command specified.", 0;

	arglist args = arglist(argv + 1, argv + argc);
	auto it_interactive = std::find_if(args.begin(), args.end(), [](const std::string &s) {
		return s == "--interactive" || s == "-i";
	});

	// Non-interactive mode: execute command and exit
	if (it_interactive == args.end())
		return execute_command(args.begin(), args.end());

	// Interactive mode: after executing the specified command,
	// prompt the user for additional commands.

	// Erase the interactive flag - we don't want to pass it to the command
	args.erase(it_interactive);

	// Is the first word a recognized command keyword?
	if (commands.find(args[0]) != commands.end())
		commands[args[0]](args.begin() + 1, args.end());
	else
		return std::cerr << "Unrecognized " << (args[0][0] == '-' ? "option\n" : "command.\n"), -1;

	// After executing the specified command (if any), prompt the user for more.
	interactive();
}