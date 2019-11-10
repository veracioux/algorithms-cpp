#include "Logic.h"

namespace logic
{
	// Implementation of class Implicant

	Implicant::Implicant(ullong form, ullong mask) : form(form & mask),
													 mask(mask)
	{}

	Implicant::Implicant(const std::string &literal, unsigned char nVariables) : form(~0ULL), mask(0)
	{
		if (nVariables == 0)
			nVariables = 1 + (*std::max_element(literal.begin(), literal.end(), [](char x, char y) {
				return x < y || x == '\'';
			}) - 'A');
		else if (nVariables > LONG_LONG_SIZE)
			throw std::domain_error("Illegal number of variables");
		for (int i = 0; i < literal.size(); ++i)
		{
			char letter = literal[i];
			if (letter != '\'' && (letter - 'A' < 0 || letter - 'A' >= nVariables)) // Invalid form
				throw std::domain_error("Implicant can contain only letters or \' symbols");
			else if (letter != '\'') // This character is a variable
			{
				ullong sampler = 1ULL << (nVariables - 1 - letter + 'A');
				mask |= sampler; // Adds this variable to the mask
				if (i < literal.size() - 1 && literal[i + 1] == '\'') // This variable appears negated
					form ^= sampler;
			}
		}
		form &= mask;
	}

	Implicant::Implicant(ullong form, unsigned char nVariables)
	{
		if (nVariables == 0)
			nVariables = __numberOfVariables;
		if (nVariables > LONG_LONG_SIZE)
			throw std::domain_error("Illegal number of variables");
		mask = (1ULL << nVariables) - 1;
		Implicant::form = form & mask;
	}

	std::string Implicant::ToLiteral(unsigned char n) const
	{
		if (n > LONG_LONG_SIZE)
			throw std::domain_error("Illegal number of variables");

		std::string literal;
		// Sampler contains a 1 only on the bit of interest
		ullong sampler;
		if (n == 0)
			for (sampler = 1ULL << (LONG_LONG_SIZE - 1); !(sampler & mask); sampler >>= 1U);
		else
			sampler = 1ULL << (n - 1);
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

	int Implicant::GetNegations() const
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

	int Implicant::GetNumberOfSymbols() const
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
		return mask == x.mask && IsPowerOf2(form ^ x.form);
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

	// End of operators of Implicants

	Implicant Combine(Implicant x, const Implicant &y)
	{
		return x |= y;
	}

	std::set<Implicant>
	Combine(const std::set<Implicant> &implicants, bool &arePrime, std::set<Implicant> &primeImplicants)
	{
		std::set<Implicant> returnSet;
		primeImplicants.insert(implicants.begin(), implicants.end());
		arePrime = true;
		for (auto it1 = implicants.begin(); it1 != implicants.end(); ++it1)
			for (auto it2 = std::next(it1, 1); it2 != implicants.end(); ++it2)
				if (it1->CanCombineWith(*it2))
				{
					returnSet.insert(Combine(*it1, *it2));
					primeImplicants.erase(*it1);
					primeImplicants.erase(*it2);
					arePrime = false;
				}
		return returnSet;
	}

	bool AreCoveredBy(
			const std::set<Implicant> &minterms,
			const std::vector<decltype(std::set<Implicant>().begin())> &implicants)
	{
		for (auto &minterm : minterms)
		{
			for (auto &implicant : implicants)
				if (implicant->ImpliedBy(minterm))
					goto FOUND;
			return false; // goto never happened => no implicant was found that covers this minterm
			FOUND:
			continue;
		}
		return true;
	}

	std::set<std::set<Implicant>> GetMDNF(std::set<Implicant> minterms, std::set<Implicant> dontCare)
	{
		minterms.insert(dontCare.begin(), dontCare.end());
		std::set<std::set<Implicant>> returnSet;
		std::set<int> numOfLettersSet;
		std::set<Implicant> implicants = minterms, primeImplicants;
		bool arePrime = false;
		while (!arePrime)
			implicants = Combine(implicants, arePrime, primeImplicants);
		// At this point, all prime implicants have been found and placed into "primeImplicants"
		// We clear "implicants" because we will use it to store must-have implicants
		implicants.clear();
		int implicantsNumOfLetters = 0;
		/* We have to remove all minterms that are covered by only one implicant
		 * and we have to remove such implicants from "primeImplicants" for more efficient generation
		 * of DNFs. We will store those implicants in "implicants", so we can add
		 * them to our final result.
		 */
		for (auto it = minterms.begin(); it != minterms.end();)
		{
			auto minterm = *it;
			// Looks if "term" is a don't care combination - if yes, it is removed from "minterms"
			if (dontCare.find(minterm) != dontCare.end())
				it = minterms.erase(it);
			else
			{
				// Counts the number of implicants that cover "minterm". An iterator to the first such implicant is stored in "itImplicant"
				int implicantCounter = 0;
				decltype(primeImplicants.begin()) itImplicant;
				for (auto it1 = primeImplicants.begin(); it1 != primeImplicants.end(); ++it1)
					if (*it1 << minterm) // This implicant covers "minterm"
					{
						if (++implicantCounter == 2) break;
						itImplicant = it1;
					}
				// If only one implicant covers "minterm", it is added to "implicants" and "minterm" is removed from "minterms"
				if (implicantCounter == 1)
				{
					implicants.insert(*itImplicant);
					implicantsNumOfLetters += itImplicant->GetNumberOfSymbols();
					// Finds all minterms that are covered by this implicant and removes them from consideration
					for (auto it1 = minterms.begin(); it1 != minterms.end();)
						if (*itImplicant << *it1)
							it1 = minterms.erase(it1);
						else
							++it1;
					it = minterms.begin();
				}
				else ++it;
			}
		}
		// If no implicants other than must-have implicants are needed, then we can just return those must-haves
		if (minterms.size() == 0) return {implicants};
		// Now we loop through all possible combinations of our prime implicants, excluding must-have implicants
		int maxSize = primeImplicants.size(), minLetters = std::numeric_limits<int>::max();
		for (int size = 1; size <= maxSize && size < minLetters; ++size)
		{
			// Creates the base combination of prime implicants
			auto combination = CreateCombination(primeImplicants, size);
			do
			{
				int numOfLetters = implicantsNumOfLetters;
				// Tests if the current combination of implicants is a valid IDNF and forms a set accordingly
				if (AreCoveredBy(minterms, combination))
				{
					std::set<Implicant> IDNF;
					for (auto &iterImplicant : combination)
					{
						auto implicant = *iterImplicant;
						numOfLetters += implicant.GetNumberOfSymbols();
						IDNF.insert(implicant);
					}
					// If we already had an IDNF that has fewer letters, then this IDNF cannot possibly be an MDNF
					if (numOfLetters <= minLetters)
					{
						IDNF.insert(implicants.begin(), implicants.end());
						returnSet.insert(IDNF);
						numOfLettersSet.insert(numOfLetters);
					}
					if (numOfLetters < minLetters) minLetters = numOfLetters;
				}
			} while (NextCombination(combination, primeImplicants.end()));
		}
		// Now we have to remove all DNFs that contain more letters than "minLetters"
		auto itLetters = numOfLettersSet.begin();
		for (auto it = returnSet.begin(); it != returnSet.end(); ++itLetters)
			if (*itLetters > minLetters) // This DNF has more letters than necessary => it is not an MDNF
				it = returnSet.erase(it);
			else ++it;
		return returnSet;
	}

	std::string ToLiteral(std::set<Implicant> DNF, unsigned char n)
	{
		if (n > LONG_LONG_SIZE)
			throw std::domain_error("Illegal number of variables");
		else if (n == 0)
			n = __numberOfVariables;
		std::string literal;
		// Iterates through all elementary conjunctions in the given DNF
		for (auto it = DNF.begin(); it != DNF.end(); ++it)
		{
			if (it != DNF.begin()) literal += '+';
			literal += it->ToLiteral(n);
		}
		return literal;
	}

	void SetNumberOfVariables(unsigned char n)
	{
		__numberOfVariables = n;
	}
}