#pragma once

#include <vector>

namespace numeric
{
	constexpr unsigned char LONG_LONG_SIZE = 64;
}

using ullong = unsigned long long;
template<class T>
using Matrix = std::vector<std::vector<T>>;