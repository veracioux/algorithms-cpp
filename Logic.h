#ifndef ALGORITHMSCPP_LOGIC_H
#define ALGORITHMSCPP_LOGIC_H

#include <string>
#include <stdexcept>
#include <algorithm>
#include <set>
#include <vector>

#define ullong unsigned long long

// Could be implementation dependent
constexpr unsigned char LONG_LONG_SIZE = 64;

/**
 * Efficiently checks if x is a power of 2.
 */
inline bool IsPowerOf2(ullong x)
{
	return x != 0 && !(x & (x - 1));
}

namespace logic
{
	/**
	 * Wrapper class for elementary conjunctions (implicants).
	 * Contains no more than <code>LONG_LONG_SIZE</code> variables.
	 */
	class Implicant
	{
		// Form bits are guaranteed to be 0 wherever mask bits are also 0
		ullong form, mask;
	public:
		/**
		 * @param form - Binary representation of this implicant.
		 * Clear bits represent variables that are negated.
		 * @param mask - Binary number, set bits represent variables
		 * that are included.
		 *
		 * @example The implicant A'B'C (n=4) has the following \p form :
		 * 	<code>0b0010</code> , \p mask : <code>0b1110</code>
		 */
		Implicant(ullong form, ullong mask);

		/**
		 * Creates a new Implicant based on its textual/mathematical representation.
		 * @param literal - An elementary conjunction that contains letters starting from
		 * 'A' and apostrophe symbols representing negations. Repetition of symbols
		 * can result in unexpected behavior.
		 * @param[optional] nVariables - Number of variables. If this is not specified,
		 * the number of variables is automatically determined.
		 *
		 * @throw <code>std::domain_error</code> - If the literal contains letters
		 * that can't be accommodated into the number of variables given by the template parameter,
		 * or if <code>n</code> is illegal.
		 *
		 * @example[1] The string <code>"A'"</code> represents A negated.
		 * @example[2] The string <code>"AB'C"</code> is valid.
		 * @example[3] The string <code>"AB''C"</code> will result in unspecified behavior.
		 * @example[4] The string <code>"AB'C"</code> will cause an exception to be thrown if <code>n</code> is less than 3.
	 	*/
		Implicant(const std::string &literal, unsigned char nVariables = 0);

		/**
		 * Creates an implicant/minterm as specified by \p form
		 * with a mask that covers all variables.
		 * @param nVariables - Number of variables.
		 *
		 * @see <code>Implicant(unsigned long long, unsigned long long)</code>
		 */
		Implicant(ullong form, unsigned char nVariables = 0);

		// Getters

		/**
		 * @return Whether the variable of specified index is included in the implicant.
		 * @example If this implicant is ABC' and we are considering
		 * 3 variables, <code>Contains(0)</code> will return <code>true</code>.
		 * @example If this implicant is ABC' and we are considering
		 * 4 variables, <code>Contains(0)</code> will return <code>false</code>.
		 */
		bool Contains(unsigned char index) const;

		/**
		 * @return <code>true</code> if the variable of the specified index
		 * appears without a negation, <code>false</code> otherwise.
		 * @throw <code>std::logic_error</code> If the implicant does not
		 * contain the variable of the specified index.
		 */
		bool operator[](unsigned char index) const;

		/**
		 * Returns the number of negated variables in the implicant.
		 */
		int GetNegations() const;

		/**
		 * @return Binary representation of this implicant's negations.
		 */
		ullong GetForm() const;

		/**
		 * @return Binary representation of this implicant's variables.
		 */
		ullong GetMask() const;

		/**
		 * @return The number of variables in this implicant.
		 */
		int GetNumberOfSymbols() const;

		/**
		 * @param n - Number of variables in the function this implicant is part of.
		 *
		 * @return Textual representation of this implicant,
		 * starting from the letter A. Negations are represented by
		 * apostrophe characters.
		 */
		std::string ToLiteral(unsigned char n) const;

		// Relational operators

		/**
		 * @return Whether <code>this</code> is before \p x
		 * @note Implicant x is before implicant y if the following holds: <br>
		 * - They have the same mask and x.form is less than y.form <br>
		 * - x.mask > y.mask
		 */
		bool operator<(const Implicant &x) const;

		/**
		 * @return Whether this implicant can be combined with implicant \p x
		 * @note Two implicant can be combined if and only if they contain
		 * the same variables and differ by exactly one negation.
		 */
		bool CanCombineWith(const Implicant &x) const;

		/**
		 * Returns whether \p x implies <code>this</code>.
		 */
		bool ImpliedBy(const Implicant &x) const;

		/**
		 * @return Whether <code>this</code> implies \p x.
		 */
		bool operator>>(const Implicant &x) const;

		/**
		 * @return Whether \p x implies <code>this</code>.
		 * @see <code>ImpliedBy(const Implicant&)</code>
		 */
		bool operator<<(const Implicant &x) const;

		/**
		 * Combine this implicant with \p x.
		 */
		void Combine(const Implicant &x);

		/**
		 * Combine this implicant with \p x.
		 * @see <code>Combine(const Implicant&)</code>
		 */
		Implicant &operator|=(const Implicant &x);

		/**
		 * Combine this implicant with \p x.
		 * @see <code>Combine(const Implicant&)</code>
		 */
		Implicant operator|(const Implicant &x);

		/**
	     *  Write an implicant to a stream in the same format as returned by <code>ToLiteral()</code>.
	     *  @param o - Output stream.
	     *  @param x - Implicant to write out.
	     *  @return  Reference to the output stream.
	    */
		friend std::ostream &operator<<(std::ostream &o, const Implicant &x);

	};

	// Preloaded number of variables to be used in various methods.
	static unsigned char __numberOfVariables = 4;

	void SetNumberOfVariables(unsigned char n);

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
	std::set<Implicant>
	Combine(const std::set<Implicant> &implicants, bool &arePrime, std::set<Implicant> &primeImplicants);

/**
 * @return Whether the logical function specified by \p minterms is covered by \p implicants.
 * @param minterms - Set of minterms that form a logical function.
 * @param implicants - Vector of iterators that point to implicants.
 */
	bool AreCoveredBy(
			const std::set<Implicant> &minterms,
			const std::vector<decltype(std::set<Implicant>().begin())> &implicants);

/**
 * Creates a vector of iterators that point to the first \p n elements of \p container, starting with the element at <code>container.begin()</code>.
 */
	template<class Container>
	std::vector<decltype((new Container())->begin())>
	CreateCombination(const Container &container, int n, decltype((new Container())->begin()) begin)
	{
		std::vector<decltype((new Container())->begin())> returnVector(n);
		int i = 0;
		for (auto it = begin; it != container.end(); ++it, ++i)
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
 * @param minterms - Set of minterms that form a logical function. It can but need not include dontCare combinations.
 * @param[optional] dontCare - Set that contains which minterms are don't care.
 * @return Set of minimal disjunctive normal forms for the logical function given my \p minterms.
 */
	std::set<std::set<Implicant>> GetMDNF(std::set<Implicant> minterms, std::set<Implicant> dontCare = {});

/**
 * Takes a disjunctive normal form and returns its textual/mathematical representation.
 * @param DNF - Set of implicants.
 * @param n - Number of variables of the logical function.
 */
	std::string ToLiteral(std::set<Implicant> DNF, unsigned char n);
}

#endif //ALGORITHMSCPP_LOGIC_H