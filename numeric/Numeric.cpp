#include "Numeric.h"

// Harmonic number class implementation

namespace numeric
{
	HarmonicNumber::HarmonicNumber(unsigned long long n)
	{
		this->n = n;
	}

	double HarmonicNumber::operator[](unsigned long long i)
	{
		return 1. / (n - i);
	}

	unsigned long long HarmonicNumber::size()
	{
		return n;
	}

	double NthHarmonicNumber(unsigned long long n)
	{
		//TODO return KahanSum(HarmonicNumber(n));
		return 0;
	}

	double Sin(double x, unsigned int n)
	{
		if (n == 0) return x;
		x = x - int(x / _2PI) * _2PI;
		double result = x * x / ((2 * n - 1) * (2 * n - 2));
		for (unsigned int i = n - 1; i >= 1; --i)
			result = result * x * x / ((2 * i - 1) * (2 * i - 2)) + (i % 2 == 0 ? x : -x);
		return result;
	}

	double Exp(double power, unsigned int n)
	{
		if (n == 0) return 1;
		int integerPart = int(power), rem = power - integerPart;
		double result = rem / n + 1;
		for (unsigned int i = n - 1; i >= 1; --i)
			result = result * rem / i + 1;
		return result * NthPower(const_e, integerPart);
	}

	double ScalarProduct(std::vector<double> a, std::vector<double> b)
	{
		double result = 0;
		for (unsigned long i = 0; i < std::min(a.size(), b.size()); ++i)
			result = std::fma(a[i], b[i], result);
		return result;
	}

	double Epsilon()
	{
		double eps = 1;
		do
			eps /= 2;
		while (1 + eps > 1);
		return eps;
	}
}