#include "veitch.h"
#include "program.h"
#include "../logic/VeitchDiagram.h"

namespace veitch
{

int veitch(const arglist_iter &begin, const arglist_iter &end)
{
	bool minimize = true, border = true, loops = true;
	int show_n = 1;

	arglist args_minimize;
	// First we parse arguments exclusive to this command, and store any additional
	// arguments into args_minimize
	for (auto it = begin; it != end; ++it)
	{
		if (*it == "--help" || *it == "-h")
			return help(), 0;
		else if (*it == "--nominimize" || *it == "-M")
			minimize = false;
		else if (*it == "--noborder" || *it == "-b")
			border = false;
		else if (*it == "--border" || *it == "-B")
			border = true;
		else if (*it == "--show" || *it == "-s")
		{
			if (it + 1 == end || !extract_n(*++it, show_n) || show_n < 0)
				return std::cerr << "Illegal variable count.\n", -1;
		}
		else if (*it == "--show-all" || *it == "-S")
			show_n = 0;
		else if (*it == "--loops" || *it == "-L")
			loops = true;
		else if (*it == "--noloops" || *it == "-l")
			loops = false;
		else
		{
			// Arguments relating to DNF parsing and/or minimization
			args_minimize.push_back(*it);
		}
	}

	std::set<logic::DNF> dnfs;
	logic::DNF dnf, dontCare;
	unsigned char n = 0;

	eval(minimize::extract_input_variables(args_minimize.begin(), args_minimize.end(), dnf, dontCare, n))

	if (minimize)
		dnfs = logic::GetMDNF(n, dnf, dontCare);
	else
		dnfs.insert(dnf);

	if (show_n == 0)
		show_n = dnfs.size();
	int i = 0;
	for (auto it = dnfs.begin(); i < show_n && it != dnfs.end(); ++i, ++it)
		logic::VeitchDiagram(n, *it, dontCare, false).Print(std::cout, border) << '\n';

	return 0;
}

const char *help_synopsis()
{
	return "Draw a Veitch diagram based on the logical function specified as the argument.\n"
		   "The function is minimized by default. Currently supports only disjunctive normal forms.\n"
		   "Usage: veitch [OPTIONS...] expression\n";
}

const char *help_options()
{
	return "\t-m, --minterms\t\t\tFunction input is in the form of minterm indices.\n"
		   "\t-M, --nominimize\t\tDo not minimize this function.\n"
		   "\t-b, --noborder\t\t\tDo not draw table border.\n"
		   "\t-B, --border\t\t\tDraw table border.\n"
		   "\t-s, --show\t\t\t\tNumber of MDNFs that are to be drawn.\n"
		   "\t\t\t\t\t\t\tA zero value will show all MDNFs. (Default: 1)\n"
		   "\t-S, --show-all\t\t\tDraw diagrams for all MDNFs.\n"
		   "\t-L, loops\t\t\t\tDraw loops around prime implicants of the MDNF.\n"
		   "\t-l, --noloops\t\t\tDraw the diagram without loops. (currently not supported)\n"; //TODO add support
}

const char *help_examples()
{
	return "\tveitch --show 2 --variable-count ABC+ABC'+A'C \n"
		   "\tveitch -n 4 ABC+AB'C --dont-care ABC',ABC\n"
		   "\tveitch --minterms 0,1,5,10,13,14 --dont-care 2,4,9\n";
}

void help()
{
	std::cout << help_synopsis()
			  << "\nOptions:\n"
			  << "\t-h, --help\t\t\t\tShow this help list\n"
			  << help_options()
			  << minimize::help_options()
			  << "\nExamples:\n"
			  << help_examples();
}

}