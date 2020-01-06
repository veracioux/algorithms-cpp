#pragma once

#include <vector>

#include "Implicant.h"
#include "McCluskey.h"

namespace logic
{

	/**
	 * Currently supports at most 5 variables.
	 */
	class VeitchDiagram
	{
	public:
		struct Contour
		{
			/*
			 * The intersection of the specified rows and columns determines
			 * where the associated Veitch diagram contains 1's.
			 */
			std::vector<unsigned int> rows, columns;
		};
	private:

		unsigned int nVariables;
		// Elements equal to 0, 1, 2 represent 0's, 1's and x's respectively.
		Matrix<unsigned short> matrix;
		std::vector<Contour> contours;

		// Private methods

		std::vector<unsigned int> GetVariableRows(unsigned int index, bool negated) const;

		std::vector<unsigned int> GetVariableColumns(unsigned int index, bool negated) const;

		std::vector<unsigned int>
		GetIntersection(const std::vector<unsigned int> &test, int i, const Implicant &implicant, bool rows) const;

	public:

		/**
		 * Constructs a Veitch diagram for the specified number of variables
		 */
		explicit VeitchDiagram(unsigned int nVariables);

		/**
		 * Construct a Veitch diagram for the specified number of variables and
		 * filled according to the logical function specified by <code>dnf</code> and <code>dontCare</code> .
		 * Contours are constructed based on the specified form if <code>shouldMinimize = false</code>
		 * and on the minimized form otherwise. If there are multiple minimal forms, the first
		 * one returned by <code>logic::GetMDNF</code> will be used.
		 * @param nVariables - Number of variables.
		 * @param dnf - Disjunctive normal form of the logical function.
		 * @param[optional] dontCare - Minterms that represent don't care combinations.
		 * @param[optional] shouldMinimize - Whether the DNF specified by <code>dnf</code> has been previously minimized
		 * represented by this Veitch diagram. Default value is <code>false</code>.
		 */
		VeitchDiagram(unsigned int nVariables, DNF dnf, const DNF &dontCare = {}, bool shouldMinimize = false);

		/**
		 * Calls <code>VeitchDiagram(nVariables, dnf, {}, shouldMinimize)</code> to construct a Veitch diagram.
		 */
		VeitchDiagram(unsigned int nVariables, const DNF &dnf, bool shouldMinimize = false);

		// Getters

		/** @return The number of columns of this Veitch diagram. */
		ullong GetWidth() const;

		/** @return The number of columns of this Veitch diagram. */
		ullong GetHeight() const;

		unsigned int GetNumberOfVariables() const;

		Matrix<unsigned short> GetMatrixRepresentation() const;

		Contour GetContour(const Implicant &implicant) const;

		std::vector<Contour> GetContours() const;

		std::ostream &Print(std::ostream &stream, bool drawBorders = true) const;

		friend std::ostream &operator<<(std::ostream &stream, const VeitchDiagram &diagram);
	};
}


