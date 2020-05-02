#include <iostream>
#include "McCluskey.h"

namespace logic
{
	DNF Combine(const DNF &implicants, bool &arePrime, DNF &primeImplicants)
	{
		//TODO Which assumptions can I make regarding the ordering?
		DNF returnSet;
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
			const DNF &minterms,
			const std::vector<decltype(DNF().begin())> &implicants)
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

	std::set<DNF> GetMDNF(unsigned char nVariables, DNF dnf, const DNF &dontCare)
	{
		ConvertToCanonicalSumOfProducts(nVariables, dnf);
		dnf.insert(dontCare.begin(), dontCare.end());
		std::set<DNF> returnSet;
		DNF implicants = dnf, primeImplicants;
		bool arePrime = false;
		while (!arePrime)
			implicants = Combine(implicants, arePrime, primeImplicants);
		// At this point, all prime implicants have been found and placed into "primeImplicants"
		// We clear "implicants" because we will use it to store essential implicants
		implicants.clear();
		/* We have to remove all DNFs that are covered by only one implicant
		 * and we have to remove such implicants from "primeImplicants" for more efficient generation
		 * of DNFs. We will store those implicants in "implicants", so we can add
		 * them to our final result.
		 */
		for (auto it = dnf.begin(); it != dnf.end();)
		{
			auto minterm = *it;
			// Looks if "term" is a don't care combination - if yes, it is removed from "dnf"
			if (dontCare.find(minterm) != dontCare.end())
				it = dnf.erase(it);
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
				// If only one implicant covers "minterm", it is added to "implicants" and "minterm" is removed from "dnf"
				if (implicantCounter == 1)
				{
					implicants.insert(*itImplicant);
					// Finds all minterms that are covered by this implicant and removes them from consideration
					for (auto it1 = dnf.begin(); it1 != dnf.end();)
						if (*itImplicant << *it1)
							it1 = dnf.erase(it1);
						else
							++it1;
					it = dnf.begin();
				}
				else ++it;
			}
		}
		// If no implicants other than essential implicants are needed, then we can just return those essential implicants
		if (dnf.empty()) return {implicants};
		// Now we loop through all possible combinations of our prime implicants, excluding essential implicants
		int maxSize = primeImplicants.size(), minLetters = std::numeric_limits<int>::max();
		for (int size = 1; size <= maxSize && size < minLetters; ++size)
		{
			// Creates the base combination of prime implicants (increasing sequence)
			auto combination = CreateCombination(primeImplicants, size);
			do
			{
				ullong numOfLetters = 0;
				// Tests if the current combination of implicants is a valid irreducible DNF and forms a set accordingly
				if (AreCoveredBy(dnf, combination))
				{
					DNF IDNF;
					for (auto &iterImplicant : combination)
					{
						auto implicant = *iterImplicant;
						numOfLetters += implicant.GetVariableCount();
						IDNF.insert(implicant);
					}
					if (numOfLetters == minLetters) // Keep this IDNF until we find a more favorable one (if ever)
					{
						IDNF.insert(implicants.begin(), implicants.end());
						returnSet.insert(IDNF);
					}
					else if (numOfLetters < minLetters) // This IDNF has fewer letters than all others => the others are not MDNFs
					{
						minLetters = numOfLetters;
						IDNF.insert(implicants.begin(), implicants.end());
						returnSet = {IDNF};
					}
				}
			} while (NextCombination(combination, primeImplicants.end()));
		}
		return returnSet;
	}

	std::string ToLiteral(const DNF &dnf, unsigned char n)
	{
		if (n > numeric::LONG_LONG_SIZE)
			throw std::domain_error("Illegal number of variables");
		else if (n == 0)
		{
			for (auto impl : dnf)
			{
				int x = impl.GetVariableCount();
				if (x > n) n = x;
			}
		}
		std::string literal;
		// Iterates through all elementary conjunctions in the given DNF
		for (auto it = dnf.begin(); it != dnf.end(); ++it)
		{
			if (it != dnf.begin()) literal += '+';
			literal += it->ToLiteral(n);
		}
		return literal;
	}

	void PrintMDNF(unsigned char nVariables, const DNF &minterms, const DNF &dontCare)
	{
		for (auto &x : GetMDNF(nVariables, minterms, dontCare))
		{
			std::cout << ToLiteral(x, nVariables) << std::endl;
		}
	}

	void _ExtrapolateMintermsAndAdd(ullong mintermMask, const Implicant &implicant, DNF &dnf, unsigned char startIndex = 0)
	{
		if (implicant.GetMask() != mintermMask)
		{
			// This implicant is missing a variable
			ullong sampler = 1ULL << startIndex;
			// We sample all bits in the mask, looking for missing variables
			do
			{
				++startIndex;
				if ((implicant.GetMask() & sampler) == 0)
				{
					_ExtrapolateMintermsAndAdd(mintermMask, Implicant(implicant.GetForm() | sampler, implicant.GetMask() | sampler), dnf, startIndex);
					_ExtrapolateMintermsAndAdd(mintermMask, Implicant(implicant.GetForm() & ~sampler, implicant.GetMask() | sampler), dnf,
											   startIndex);
					return;
				}
				sampler <<= 1U;
			} while (sampler & mintermMask);
		}
		else
			dnf.insert(implicant);
	}

	void ExtrapolateMintermsAndAdd(unsigned char nVariables, const Implicant &implicant, DNF &dnf)
	{
		_ExtrapolateMintermsAndAdd((1ULL << nVariables) - 1, implicant, dnf);
	}

	void ConvertToCanonicalSumOfProducts(unsigned char nVariables, DNF &dnf)
	{
		DNF newDNF;
		ullong mintermMask = (1ULL << nVariables) - 1;
		for (auto x : dnf)
			_ExtrapolateMintermsAndAdd(mintermMask, x, newDNF);
		dnf = std::move(newDNF);
	}
}