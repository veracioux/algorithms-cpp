#ifndef ALGORITHMSCPP_LOGIC_H
#define ALGORITHMSCPP_LOGIC_H

#include <string>
#include <stdexcept>

#define ullong unsigned long long

/**
 * Efficiently checks if x is a power of 2.
 */
inline bool IsPowerOf2(ullong x)
{
	return !(x & (x - 1));
}

namespace logic
{
	/**
	 * Wrapper class for elementary conjunctions (implicants).
	 * Contains no more than <code>sizeof(long long)</code> variables.
	 * @tparam n - The maximum number of variables in this implicant.
	 * 	This can be more than the number of variables
	 * 	that are actually present in the implicant.
	 */
	template<int n>
	class Implicant
	{
		// Form is guaranteed to be 0 wherever mask is also 0
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
		 *
		 * @throw <code>std::domain_error</code> - If the literal contains letters
		 * that can't be accommodated into the number of variables given by the template parameter.
		 *
		 * @example[1] The string <code>"A'"</code> represents A negated.
		 * @example[2] The string <code>"AB'C"</code> is valid.
		 * @example[3] The string <code>"AB''C"</code> will result in unspecified behavior.
		 * @example[4] The string <code>"AB'C"</code> will cause an exception to be thrown if <code>n</code> is less than 3.
	 	*/
		Implicant(const std::string &literal);

		/**
		 * Creates an implicant/minterm as specified by \p form
		 * with a mask that covers all variables.
		 * @see <code>Implicant(unsigned long long, unsigned long long)</code>
		 */
		Implicant(ullong form);

		std::string ToLiteral() const;

		/**
		 * @return Whether the variable of specified index is included in the implicant
		 */
		bool Contains(char index) const;

		/**
		 * @return "true" if the variable of specified index appears without a negation,
		 * "false" otherwise
		 */
		bool operator[](char index) const;

		/**
		 * Returns the number of negations in the implicant
		 */
		int GetNegations() const;

		ullong GetForm() const;

		int GetNumberOfSymbols() const;

		bool operator<(const Implicant &x) const;

		bool CanCombineWith(const Implicant &x) const;

		/**
		 * Returns whether <code>x</code> implies "this".
		 */
		bool ImpliedBy(const Implicant &x) const;

		bool operator>>(const Implicant &x) const;

		bool operator<<(const Implicant &x) const;

		void Combine(const Implicant &x);

		Implicant &operator|=(const Implicant &x);

		friend std::ostream &operator<<(std::ostream &o, const Implicant &x);
	};

	template<int n>
	Implicant<n> Combine(const Implicant<n> &x, const Implicant<n> &y);

}

#endif //ALGORITHMSCPP_LOGIC_H