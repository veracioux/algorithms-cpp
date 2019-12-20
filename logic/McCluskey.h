#pragma once

#include "Implicant.h"

namespace logic
{
	/**
	 * @return The result of combining implicants \p x and \p y.
	 */
	Implicant Combine(Implicant x, const Implicant &y);

	/**
	 * Performs one iteration of McCluskey.
	 * @param implicants - Contains all implicants from the previous iteration (or minterms).
	 * @param arePrime - Will be set to <code>true</code> if the result contains all prime implicants, <code>false</code> otherwise.
	 * @param primeImplicants - Will contain implicants that cannot be combined with other implicants.
	 * @return Set of simplified implicants.
	 */
	DNF Combine(const DNF &implicants, bool &arePrime, DNF &primeImplicants);

	/**
	 * @return Whether the logical function specified by \p minterms is covered by \p implicants.
	 * @param minterms - Set of minterms that form a logical function.
	 * @param implicants - Vector of iterators that point to implicants.
	 */
	bool AreCoveredBy(
			const DNF &minterms,
			const std::vector<decltype(DNF().begin())> &implicants);

	/**
	 * Creates a vector of iterators that point to the first \p n elements of \p container, starting with the element at <code>container.begin()</code>.
	 */
	template<class Container>
	std::vector<decltype((new Container())->begin())>
	CreateCombination(const Container &container, int n, decltype((new Container())->begin()) begin)
	{
		std::vector<decltype((new Container())->begin())> returnVector(n);
		auto it = begin;
		for (int i = 0; i < n; ++it, ++i)
			returnVector[i] = it;
		return returnVector;
	}

	/**
	 * Creates a vector of iterators that point to the first \p n elements of \p container.
	 */
	template<class Container>
	std::vector<decltype((new Container())->begin())> CreateCombination(const Container &container, int n)
	{
		return CreateCombination(container, n, container.begin());
	}

	/**
	 * Finds the next combination of elements of a set and stores it in \p combination.
	 * @param end - Iterator that points to the last element of the set.
	 * @return Whether the the combination could be advanced.
	 */
	template<class Iterator>
	bool NextCombination(std::vector<Iterator> &combination, const Iterator &end)
	{
		int k = combination.size(), i = k - 1;
		// Goes through all elements until it finds one that can be incremented
		for (; i >= 0 && std::distance(combination[i], end) <= k - i; --i);
		if (i != -1)
		{
			// Found an element that can be advanced
			++combination[i];
			for (int j = i + 1; j < k; ++j)
				combination[j] = std::next(combination[i], j - i);
			return true;
		}
		else
			return false;
	}

	/**
	 * Inserts all minterms that can be combined to \p implicant, into \p dnf.
	 */
	void ExtrapolateMintermsAndAdd(unsigned char nVariables, const Implicant &implicant, DNF &dnf);

	/**
	 * Adds the missing terms in order to get a sum of products form.
	 * @param dnf - The input and output DNF.
	 * @param[optional] - nVariables The number of variables of the function \p dnf represents.
	 * If this is not specified, the number of variables will be automatically determined.
	 */
	void ConvertToCanonicalSumOfProducts(unsigned char nVariables, DNF &dnf);

	/**
	 * @param nVariables Number of variables of the logical function.
	 * @param dnf - Base disjunctive normal form that constitutes a logical function.
	 * @param[optional] dontCare - Set that contains which minterms are don't care. Any elements must be minterms.
	 * @return Set of minimal disjunctive normal forms for the logical function given by \p minterms and \p dontCare.
	 */
	std::set<DNF> GetMDNF(unsigned char nVariables, DNF dnf, const DNF &dontCare = {});

	/**
	 * Takes a disjunctive normal form and returns its textual/mathematical representation.
	 * @param dnf - Set of implicants.
	 * @param n - Number of variables of the logical function.
	 */
	std::string ToLiteral(const DNF &dnf, unsigned char n);

	void PrintMDNF(unsigned char nVariables, const DNF &minterms, const DNF &dontCare = {});

}