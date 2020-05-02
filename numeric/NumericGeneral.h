#pragma once

#include "../symbols.h"

namespace numeric
{
	/**
	 * Efficiently test if x is a power of 2.
	 */
	inline bool IsPowerOf2(ullong x)
	{
		return x != 0 && !(x & (x - 1));
	}
}