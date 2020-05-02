#pragma once

#include <sstream>

#include "program.h"
#include "../logic/Implicant.h"
#include "../logic/McCluskey.h"

namespace minimize
{

int minimize(
		const arglist_iter &begin, const arglist_iter &end,
		std::set<logic::DNF> *result, logic::DNF *result_dontCare = nullptr);

int minimize(const arglist_iter &begin, const arglist_iter &end);

const char *help_synopsis();

const char *help_options();

const char *help_examples();

void help();

/**
 * Convert a string containing a literal mathematical expression
 * into a DNF. If n=0, automatically determine the number of
 * variables and store that number into n.
 * @param s - String to read from
 * @param n - Variable count
 * @param dnf - Output DNF
 * @param delimiter - Delimiter used to separate implicants
 * @return Whether the operation was successful
 */
bool string_to_DNF(const std::string &s, unsigned char &n, logic::DNF &dnf, char delimiter = '+');

/**
 * Convert a string containing a list of minterm indices into a DNF.
 * The minterm indices are separated by a comma. If n=0, automatically
 * determine the number of variables and store that number into n.
 * @param s - String to read from
 * @param dnf - Output DNF
 * @param n - Variable count
 * @return Whether the operation was successful
 */
bool index_list_to_DNF(const std::string &s, logic::DNF &dnf, unsigned char &n);

/**
 * Helper function. Parse arguments to generate the logical expression
 * to minimize, don't care combinations and variable count.
 */
int extract_input_variables(const arglist_iter &begin, const arglist_iter &end, logic::DNF &dnf, logic::DNF &dontCare, unsigned char &n);

/**
 * Helper function. Convert input strings into the DNF to be minimized and
 * don't care combinations respectively. Also print any error messages.
 * @return 0 on success
 */
int extract_DNFs(
		bool minterms, const std::string *expr, const std::string *expr_dontCare,
		logic::DNF &dnf, logic::DNF &dontCare, unsigned char &n);

}
