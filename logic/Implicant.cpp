#include "Implicant.h"
#include "../numeric/NumericGeneral.h"

namespace logic
{
// Implementation of class Implicant

Implicant::Implicant(ullong form, ullong mask) : form(form & mask),
												 mask(mask)
{}

Implicant::Implicant(const std::string &literal, unsigned char nVariables) : form(~0ULL), mask(0)
{
	if (nVariables == 0)
	{
		// Automatically determine the number of variables based on
		// the lexicographically largest letter in the string
		nVariables = 1 + (*std::max_element(literal.begin(), literal.end(), [](char x, char y) {
			return x < y || x == '\'';
		}) - 'A');
	}
	else if (nVariables > numeric::LONG_LONG_SIZE)
		throw std::domain_error("Illegal number of variables");
	for (int i = 0; i < literal.size(); ++i)
	{
		char letter = literal[i];
		if (letter != '\'' && (letter - 'A' < 0 || letter - 'A' >= nVariables))
		{ // Invalid form
			throw std::domain_error("Implicant can contain only letters or \' symbols");
		}
		else if (letter != '\'') // This character is a variable
		{
			ullong sampler = 1ULL << (ullong) (nVariables - 1 - letter + 'A');
			// Adds this variable to the mask
			mask |= sampler;
			// This variable appears negated
			if (i < literal.size() - 1 && literal[i + 1] == '\'')
				form ^= sampler;
		}
	}
	form &= mask;
}

Implicant::Implicant(ullong form, unsigned char nVariables)
{
	if (nVariables == 0 || nVariables > numeric::LONG_LONG_SIZE)
		throw std::domain_error("Illegal number of variables");
	mask = full_mask(nVariables);
	Implicant::form = form & mask;
}

std::string Implicant::ToLiteral(unsigned char n) const
{
	if (n > numeric::LONG_LONG_SIZE)
		throw std::domain_error("Illegal number of variables");
	std::string literal;
	// Sampler contains a 1 only on the bit of interest
	ullong sampler;
	if (n == 0)
	{
		// Find the first 1 bit in the mask. This bit
		// represents the variable A
		for (sampler = 1ULL << ullong(numeric::LONG_LONG_SIZE - 1); !(sampler & mask); sampler >>= 1U);
	}
	else
		sampler = 1ULL << ullong(n - 1);
	char c = 'A';
	do
	{
		// Checks if n-th mask bit is 1: if yes, this variable is included.
		if (mask & sampler)
			(literal += c) += form & sampler ? "" : "'";
		++c;
	} while (sampler >>= 1U);
	return literal;
}

bool Implicant::Contains(unsigned char index) const
{
	return mask & (1ULL << index);
}

bool Implicant::operator[](unsigned char index) const
{
	if (!Contains(index))
		throw std::logic_error("Variable does not appear in this implicant");
	return form & (1ULL << index);
}

int Implicant::GetNegationCount() const
{
	int num = 0;
	ullong sampler = 1LL;
	do
	{
		// Checks if n-th mask bit is 1: if yes, this variable is included.
		if (mask & sampler && !(form & sampler))
			++num;
	} while (sampler <<= 1U);
	return num;
}

ullong Implicant::GetForm() const
{
	return form;
}

ullong Implicant::GetMask() const
{
	return mask;
}

int Implicant::GetVariableCount() const
{
	int count = 0;
	ullong mask = Implicant::mask;
	while (mask)
	{
		++count;
		mask &= mask - 1;
	}
	return count;
}

bool Implicant::operator<(const Implicant &x) const
{
	return (mask == x.mask && form < x.form) || (mask > x.mask);
}

bool Implicant::CanCombineWith(const Implicant &x) const
{
	return mask == x.mask && numeric::IsPowerOf2(form ^ x.form);
}

bool Implicant::ImpliedBy(const Implicant &x) const
{
	return (mask & (~x.mask | (form ^ x.form))) == 0;
}

void Implicant::Combine(const Implicant &x)
{
	if (!CanCombineWith(x))
		throw std::logic_error("Implicants are incompatible and cannot be combined");
	mask = (form ^ x.form) ^ mask;
	form = form & mask;
}

Implicant &Implicant::operator|=(const Implicant &x)
{
	Combine(x);
	return *this;
}

bool Implicant::operator>>(const Implicant &x) const
{
	return x.ImpliedBy(*this);
}

bool Implicant::operator<<(const Implicant &x) const
{
	return ImpliedBy(x);
}

// End of implementation of class Implicant

// Operators with Implicants

std::ostream &operator<<(std::ostream &o, const Implicant &x)
{
	return o << x.ToLiteral(0);
}

Implicant Implicant::operator|(const Implicant &x)
{
	//TODO can achieve marginal performance improvement, but too lazy.
	return Implicant(*this) |= x;
}

// Other functions

Implicant Combine(Implicant x, const Implicant &y)
{
	return x |= y;
}

ullong full_mask(unsigned char n)
{
	return n == 0 ? ~0ULL : (1ULL << n) - 1;
}

}