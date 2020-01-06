#pragma once

#include <string>
#include <stdexcept>
#include <algorithm>
#include <set>
#include <vector>

#include "../symbols.h"
#include "symbols.h"

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
		 * @example The implicant A'B'C (n=4) has the following <code>form: 0b0010</code> , <code>mask: 0b1110</code>
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
		 * @see <code>Implicant(const std::string &, unsigned char)</code>;
		 */
		Implicant(const char *literal, unsigned char nVariables = 0) : Implicant(std::string(literal), nVariables)
		{}

		/**
		 * Creates an implicant/minterm as specified by <code>form</code>
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
		 * Return the number of negated variables in the implicant.
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
		 * @return Whether <code>this</code> is before <code>x</code>
		 * @note Implicant x is before implicant y if the following holds: <br>
		 * - They have the same mask and x.form is less than y.form <br>
		 * - x.mask > y.mask
		 */
		bool operator<(const Implicant &x) const;

		/**
		 * @return Whether this implicant can be combined with implicant <code>x</code>
		 * @note Two implicant can be combined if and only if they contain
		 * the same variables and differ by exactly one negation.
		 */
		bool CanCombineWith(const Implicant &x) const;

		/**
		 * Return whether <code>x</code> implies <code>this</code>.
		 */
		bool ImpliedBy(const Implicant &x) const;

		/**
		 * @return Whether <code>this</code> implies <code>x</code>.
		 */
		bool operator>>(const Implicant &x) const;

		/**
		 * @return Whether <code>x</code> implies <code>this</code>.
		 * @see <code>ImpliedBy(const Implicant&)</code>
		 */
		bool operator<<(const Implicant &x) const;

		/**
		 * Combine this implicant with <code>x</code>.
		 */
		void Combine(const Implicant &x);

		/**
		 * Combine this implicant with <code>x</code>.
		 * @see <code>Combine(const Implicant&)</code>
		 */
		Implicant &operator|=(const Implicant &x);

		/**
		 * Combine this implicant with <code>x</code>.
		 * @see <code>Combine(const Implicant&)</code>
		 */
		Implicant operator|(const Implicant &x);

		/**
		 * @return The same as <code>ToLiteral</code>, using the pre-specified number of variables.
		 */
		operator std::string();

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


}