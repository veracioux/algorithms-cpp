#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <stack>
#include <algorithm>
#include <initializer_list>

namespace program
{
	// Class prototypes

	class Program;

	class Command;

	class CommandHelp;

	class Option;

	class Option //TODO make template that takes any number of arguments
	{
		// The name should contain dash(es)
		std::string name, description;

		void (*onApply)() = [] {};

	public:
		//TODO
		void Apply()
		{
			onApply();
		}

		void SetOnApply(void (*function)())
		{
			onApply = function;
		}
	};

	class Command
	{
	protected:

		std::string name, msgHelp, msgExcessArguments = "Error: Too many arguments\n";
		std::vector<Option> options;

		void (*onApply)() = [] {};
		//TODO Container with arguments

	public:

		Command()
		{

		}

		Command(const std::string &name) : name(name)
		{}

		Command(const char *name) : Command(std::string(name))
		{}

		void ReadArguments(std::istream &stream);

		// Setters

		void SetName(const std::string &name)
		{
			Command::name = name;
		}

		void SetHelpMessage(const std::string &message)
		{
			msgHelp = message;
		}

		void SetOnApply(void (*function)())
		{
			onApply = function;
		}

		// Getters

		const std::string &GetName() const
		{
			return name;
		}

		std::string GetHelpMessage()
		{
			return msgHelp;
		}

		bool IsOptionless()
		{
			return options.empty();
		}

		//TODO placeholder implementation for debugging
		bool IsArgumentless()
		{
			return true;
		}

		void Apply()
		{
			onApply();
		}

		bool ExtractAttributes(std::istream &stream, std::string *message = nullptr);
	};

	// Some common pre-defined commands

	class CommandHelp : public Command
	{
	public:
		CommandHelp();
	};

	class CommandQuit : public Command
	{
	public:
		CommandQuit();
	};

	class Program
	{
		std::vector<Command> commands;
		std::string promptMsg, promptSign = "> ", msgHelp, msgQuit = "Quitting", msgError = "Error: Unrecognized command.\n";
	private:
		std::ostream &outputStream;
		std::istream &inputStream;
		// When this variable is set to false, this program will be scheduled to stop
		bool running = true;

		// This attribute is there for commands such as CommandHelp and CommandQuit to reference the current program
		static std::stack<Program *> programStack;
	public:

		/**
		 * Create a new instance of Program.
		 * @param out - The stream that will be used for output of messages by this program.
		 * @param in - The stream that will be used to read commands.
		 */
		Program(std::ostream &out, std::istream &in) : outputStream(out), inputStream(in)
		{}

		/**
		 * Add the specified command to the list of recognized commands for this program.
		 * @throw <code>std::logic_error</code> - If a command of the same name is already registered.
		 */
		Program &RegisterCommand(const Command &cmd);

		/**
		 * Add the specified commands to the list of recognized commands for this program.
		 * @throw <code>std::logic_error</code> - If a command of the same name is already registered.
		 * @note If a command in <code>commands</code> is encountered that is already registered, an exception will be thrown.
		 * However, the commands that have been registered thus far will remain, i.e. this method is not exception-safe.
		 */
		Program &RegisterCommands(std::initializer_list<const Command> commands)
		{
			for (auto &cmd : commands)
				RegisterCommand(cmd);
			return *this;
		}

		/**
		 * Attempts to extract a command matching cmd from the input stream used by this program.
		 * @return Whether the input represents a valid command.
		 */
		bool ExtractCommand(Command *&cmd);

		void Start();

		/**
		 * Schedule this program to stop as soon as any active commands have been processed.
		 */
		void Stop()
		{
			running = false;
		}

		// Getters

		const std::string &GetHelpMessage() const
		{
			return msgHelp;
		}

		const std::string &GetQuitMessage() const
		{
			return msgQuit;
		}

		const std::string &GetPromptMessage() const
		{
			return promptMsg;
		}

		const std::string &GetErrorMessage() const
		{
			return msgError;
		}

		const std::string &GetPromptSign() const
		{
			return promptSign;
		}

		// Setters

		void SetHelpMessage(const std::string &message)
		{
			msgHelp = message;
		}

		void SetQuitMessage(const std::string &message)
		{
			msgQuit = message;
		}

		void SetPromptMessage(const std::string &message)
		{
			promptMsg = message;
		}

		void SetErrorMessage(const std::string &message)
		{
			msgError = message;
		}

		void SetPromptSign(const std::string &text)
		{
			promptSign = text;
		}

		friend class Command;

		friend class CommandHelp;

		friend class CommandQuit;
	};

	/**
	 * Consume all consecutive whitespace characters. If a newline character is encountered, the process stops.
	 * The newline character is not consumed.
	 *
	 * @param stream The input stream from which to read.
	 * @return <code>true</code> if the input text contains only spaces excluding newline characters, <br>
	 * 		   <code>false</code> otherwise.
	 */
	bool ConsumeWhitespaceBeforeNewline(std::istream &stream);

}