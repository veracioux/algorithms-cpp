#pragma once

#include <algorithm>
#include <iostream>

#include "../logic/McCluskey.h"
#include "../logic/VeitchDiagram.h"

namespace mccluskey_prog
{
	int main();

	bool ShowHelp(const std::string &command);

	void ShowHelp();

	logic::DNF StringToDNF(const std::string &s, unsigned char n);

}