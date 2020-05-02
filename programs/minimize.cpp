#include "minimize.h"
#include "program.h"

namespace minimize
{

const char *help_synopsis()
{
	return "Minimize a logical function specified in a disjunctive normal form.\n"
		   "Usage: minimize [OPTIONS...] expression\n";
}

const char *help_options()
{
	return "\t-m, --minterms\t\t\tSpecify the expression and don't-cares as a list of\n"
		   "\t\t\t\t\t\t\tminterm indices separated by comma\n"
		   "\t-e, --expression\t\tSpecify a literal mathematical expression (Default)\n"
		   "\t-d, --dont-care\t\t\tSpecify don't care combinations separated by comma\n"
		   "\t-n, --variable-count\tNumber of variables\n";
}

const char *help_examples()
{
	return "\tminimize ABC+ABC'+A'C\n"
		   "\tminimize -n 4 ABC+AB'C --dont-care ABC' ABC\n"
		   "\tminimize --minterms -d 0,2,3 0,1,5,10,13,14 \n";
}

void help()
{
	std::cout << help_synopsis()
			  << "\nOptions:\n"
			  << "\t-h, --help\t\t\t\tShow this help list\n"
			  << help_options()
			  << "\nExamples:\n"
			  << help_examples();
}

bool index_list_to_DNF(const std::string &s, logic::DNF &dnf, unsigned char &n)
{
	// Should n be determined automatically?
	bool autoN = n == 0;
	auto stream = std::istringstream(s);
	std::set<ullong> indices{};
	while (!stream.eof())
	{
		ullong x;
		if (!(stream >> x) || (!stream.eof() && stream.get() != ','))
			return false;
		if (autoN)
			while (x >= (1ULL << n) || n == 0) ++n;
		indices.insert(x);
	}
	for (auto x : indices)
		dnf.insert(logic::Implicant(x, n));
	return true;
}

bool string_to_DNF(const std::string &s, unsigned char &n, logic::DNF &dnf, char delimiter)
{
	try
	{
		if (n == 0)
		{ // Scan the string for the largest letter
			for (auto c : s)
				if (c != delimiter && c != '\'' && std::toupper(c) - 'A' + 1 > n)
					n = std::toupper(c) - 'A' + 1;
		}
		if (n > numeric::LONG_LONG_SIZE)
			return false;
		int j = 0;
		for (int i = 0; i < s.size(); ++i)
			if (s[i] == delimiter)
			{
				dnf.insert(logic::Implicant(s.substr(j, i - j), n));
				j = i + 1;
			}
		dnf.insert(logic::Implicant(s.substr(j), n));
	}
	catch (std::domain_error &e)
	{
		return false;
	}
	return true;
}

int minimize(const arglist_iter &begin, const arglist_iter &end, std::set<logic::DNF> *result, logic::DNF *result_dontCare)
{
	logic::DNF dnf, dontCare;
	unsigned char n = 0;

	eval(extract_input_variables(begin, end, dnf, dontCare, n))

	if (result != nullptr)
	{ // We have called this function from another command
		*result = logic::GetMDNF(n, dnf, dontCare);
		if (result_dontCare != nullptr)
			*result_dontCare = dontCare;
		return 0;
	}

	auto mdnfs = logic::GetMDNF(n, dnf, dontCare);
	std::cout << "The following MDNFs have been found:\n";
	for (auto &mdnf : mdnfs)
		std::cout << logic::ToLiteral(mdnf, n) << '\n';
	return 0;
}

int minimize(const arglist_iter &begin, const arglist_iter &end)
{
	return minimize(begin, end, nullptr);
}

int extract_input_variables(const arglist_iter &begin, const arglist_iter &end, logic::DNF &dnf, logic::DNF &dontCare, unsigned char &n)
{
	bool minterms = false;
	int n_expressions = 0;
	std::string *expr = nullptr, *expr_dontCare = nullptr;
	// Extract all arguments
	for (auto it = begin; it != end; ++it)
	{
		if (*it == "--help" || *it == "-h")
			return help(), true;
		else if (*it == "--minterms" || *it == "-m")
			minterms = true;
		else if (*it == "--expression" || *it == "-e")
			minterms = false;
		else if (*it == "--variable-count" || *it == "-n")
		{
			if (it + 1 == end || !extract_n(*++it, n) || n > numeric::LONG_LONG_SIZE)
				return std::cerr << "Illegal variable count.\n", -1;
		}
		else if (*it == "--dont-care" || *it == "-d")
		{
			if (++it == end)
				return std::cerr << "No don't care combinations specified.\n", -1;
			expr_dontCare = &*it;
		}
		else
		{
			if ((*it)[0] == '-')
				return std::cerr << "Unknown option: " << *it << '\n', -1;
			n_expressions++, expr = &*it;
		}
	}

	// All words in the arg list are either options, their arguments or the argument of the command itself
	// If the user entered multiple command arguments or none, it was probably by mistake.
	if (n_expressions == 0)
		return std::cerr << "Missing expression.\n", -1;
	else if (n_expressions > 1)
		return std::cerr << "Extra arguments.\n", -1;

	// Extract all required DNF objects
	eval(extract_DNFs(minterms, expr, expr_dontCare, dnf, dontCare, n))

	return 0;
}

int extract_DNFs(
		bool minterms, const std::string *expr, const std::string *expr_dontCare,
		logic::DNF &dnf, logic::DNF &dontCare, unsigned char &n)
{
	// Read the command argument, i.e. the expression that is to be minimized
	if (!(minterms ? index_list_to_DNF(*expr, dnf, n) : string_to_DNF(*expr, n, dnf)))
		return std::cerr << "Specified DNF is of illegal format.\n", -1;
	// Read the specified don't care combinations
	if (expr_dontCare != nullptr && !(minterms ? index_list_to_DNF(*expr_dontCare, dontCare, n)
											   : string_to_DNF(*expr_dontCare, n, dontCare, ',')))
	{
		return std::cerr << "Illegal don't care combinations specified.\n", -1;
	}
	return 0;
}

}
