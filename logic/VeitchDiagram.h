#pragma once

#include <vector>

#include "Implicant.h"

namespace logic
{
	/**
	 * Currently supports at most 5 variables.
	 */
	class VeitchDiagram
	{
		unsigned int nVariables;
		Matrix<bool> matrix;

		// Private methods

		std::vector<unsigned int> GetVariableRows(unsigned int index, bool negated) const;

		std::vector<unsigned int> GetVariableColumns(unsigned int index, bool negated) const;

		std::vector<unsigned int>
		GetIntersection(const std::vector<unsigned int> &test, int i, const Implicant &implicant, bool rows) const;

	public:

		struct Contour
		{
			std::vector<unsigned int> rows, columns;
		};

		/**
		 * Constructs a Veitch diagram for the specified number of variables
		 */
		explicit VeitchDiagram(unsigned int nVariables);

		/**
		 * Constructs a Veitch diagram for the specified number of variables and
		 * filled according to the logical function specified by \p dnf .
		 * @param nVariables - Number of variables.
		 * @param dnf - Disjunctive normal form of the logical function
		 * represented by this Veitch diagram.
		 */
		explicit VeitchDiagram(unsigned int nVariables, const DNF &dnf);

		// Getters

		/** @return The number of columns of Veitch diagram. */
		ullong GetWidth() const;

		/** @return The number of columns of Veitch diagram. */
		ullong GetHeight() const;

		unsigned int GetNumberOfVariables() const;

		Matrix<bool> GetMatrixRepresentation() const;

	};
}


