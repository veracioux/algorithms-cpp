#include "Program.h"

namespace program
{
	std::stack<Program *> Program::programStack = {};

	void Program::Start()
	{
		programStack.push(this);
		outputStream << promptMsg;
		while (running)
		{
			outputStream << promptSign;
			Command *cmd;
			if (!ExtractCommand(cmd))
			{
				outputStream << msgError;
				inputStream.ignore(100000, '\n');
				continue;
			}
			std::string message;
			if (!cmd->ExtractAttributes(inputStream, &message))
			{
				outputStream << message;
				continue;
			}
			outputStream << "Successful command: " << cmd->GetName() << "\n";
			cmd->Apply();
		}
		programStack.pop();
	}

	bool Program::ExtractCommand(Command *&cmd)
	{
		// Eat up all whitespace characters. If '\n' is detected, extraction will be considered unsuccessful and return false.
		if (ConsumeWhitespaceBeforeNewline(inputStream))
			return false;
		// Extract the command from the input stream
		std::string extractedText;
		inputStream >> extractedText;
		// Find the corresponding Command object in the list of registered commands
		auto it = std::find_if(commands.begin(), commands.end(), [&](const Command &cmd) {
			return cmd.GetName() == extractedText;
		});
		if (it == commands.end()) // The input command is not registered
			return false;
		cmd = &*it;
		return true;
	}

	Program &Program::RegisterCommand(const Command &cmd)
	{
		if (std::find_if(commands.begin(), commands.end(), [&](const Command &_cmd) {
			return _cmd.GetName() == cmd.GetName();
		}) != commands.end())
			throw std::logic_error("The specified command is already registered in this program.");
		commands.push_back(cmd);
		return *this;
	}

	bool Command::ExtractAttributes(std::istream &stream, std::string *message)
	{
		// The parameter message is a pointer instead of a reference, because this allows it to be an optional argument
		if (IsOptionless() && IsArgumentless())
			if (!ConsumeWhitespaceBeforeNewline(stream))
			{
				// There should be nothing on this line except the command identifier and whitespace characters
				if (message != nullptr)
					*message = msgExcessArguments;
				stream.ignore(100000, '\n');
				return false;
			}
		stream.ignore(100000, '\n');
		return true;
	}

	bool ConsumeWhitespaceBeforeNewline(std::istream &stream)
	{
		while (std::isspace(stream.peek()))
		{
			if (stream.peek() == '\n')
				return true;
			stream.get();
		}
		return false;
	}

	CommandHelp::CommandHelp() : Command("help")
	{
		onApply = [] {
			Program &program = *Program::programStack.top();
			program.outputStream << program.GetHelpMessage();
		};
	}

	CommandQuit::CommandQuit() : Command("quit")
	{
		onApply = [] {
			Program &program = *Program::programStack.top();
			program.outputStream << program.GetQuitMessage();
			program.running = false;
		};
	}
}