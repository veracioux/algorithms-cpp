#pragma once

#include <type_traits>
#include <cmath>
#include <vector>
#include <iostream>
#include <iomanip>

#include "../symbols.h"

#define EPSILON (1e-15)
#define ullong unsigned long long
#define ELEMENT_TYPE(iterator) typename std::remove_reference<decltype(*iterator)>::type

// Some typedefs

using DoubleMatrix = Matrix<double>;


const double PI = 4 * std::atan(1.), _2PI = 2 * PI;
const double const_e = std::exp(1);

namespace numeric
{
	inline bool EpsilonEqual(double a, double b)
	{
		return std::abs(a - b) < EPSILON;
	}

//TODO
	template<class Iterator>
	auto KahanSum(Iterator begin, const Iterator &end) -> ELEMENT_TYPE(begin)
	{
		ELEMENT_TYPE(begin) sum = 0, previousError = 0, y, t;
		for (; begin != end; ++begin)
		{
			y = *begin - previousError;
			t = sum + y;
			previousError = (t - sum) - y;
			sum = t;
		}
		return sum;
	}

	class HarmonicNumber
	{
		ullong n;
	public:
		HarmonicNumber(ullong n);

		double operator[](ullong i);

		unsigned long long size();
	};

	double NthHarmonicNumber(unsigned long long n);

	template<class T>
	T NthPower(T x, unsigned long long n)
	{
		if (n == 0) return 1;
		if (n % 2)
			return NthPower(x, n - 1) * x;
		else
		{
			T tmp = NthPower(x, n / 2);
			return tmp * tmp;
		}
	}

	// Maclaurin series expansions
	double Sin(double x, unsigned int n = 5);

	double Exp(double power, unsigned int n = 5);

	// Uses function fma (fused multiply-add)
	double ScalarProduct(std::vector<double> a, std::vector<double> b);

	template<class T>
	std::ostream &operator<<(std::ostream &o, const std::vector<T> &v)
	{
		for (const auto &x : v)
			o << x << ' ';
		return o;
	}

	template<class T>
	std::ostream &operator<<(std::ostream &o, const Matrix<T> &m)
	{
		for (int i = 0; i < m.size(); ++i)
		{
			if (i != 0) std::cout << std::endl;
			for (auto x : m[i])
				o << std::setw(10) << x;
		}
		return o;
	}
}