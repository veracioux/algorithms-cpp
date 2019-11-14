#pragma once

#include <set>

#include "Implicant.h"

namespace logic
{
	class Implicant;

	/** Disjunctive normal form */
	using DNF = std::set<Implicant>;
}