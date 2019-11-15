#include "VeitchDiagram.h"

namespace logic
{
	// Need to change this some time in the future. The inelegance is unbearable.
	std::vector<unsigned int> rowLookupTable[5][5] = {
			// A	 B   C   D    E
			{{0}}, // n=1
			{{0}},  // n=2
			{{0}}, // n=3
			{{0, 1}, {}, {}, {1, 2}}, // n=4
			{{0, 1}, {}, {}, {}, {1, 2}} //n=5
	};
	std::vector<unsigned int> columnLookupTable[5][5] = {
			// A	 B   C   D    E
			{}, // n=1
			{{}, {0}},  // n=2
			{{}, {0, 1},       {1, 2}}, // n=3
			{{}, {0, 1},       {1, 2}}, // n=4
			{{}, {0, 1, 4, 5}, {1, 2, 5, 6}, {1, 2}, {0, 1, 2, 3}} //n=5
	};

	// Indices: 0=A, 1=B, ...
	std::vector<unsigned int> VeitchDiagram::GetVariableRows(unsigned int index, bool negated) const
	{
		if (!negated)
			return rowLookupTable[nVariables - 1][index];
		else
		{
			auto rows = rowLookupTable[nVariables - 1][index];
			if (rows.empty()) return rows;
			std::vector<unsigned int> returnVec;
			for (int i = 0; i < GetHeight(); ++i)
				if (std::find(rows.begin(), rows.end(), i) == rows.end())
					returnVec.push_back(i);
			return returnVec;
		}
	}

	std::vector<unsigned int> VeitchDiagram::GetVariableColumns(unsigned int index, bool negated) const
	{
		if (!negated)
			return columnLookupTable[nVariables - 1][index];
		else
		{
			auto columns = columnLookupTable[nVariables - 1][index];
			if (columns.empty()) return columns;
			std::vector<unsigned int> returnVec;
			for (int i = 0; i < GetHeight(); ++i)
				if (std::find(columns.begin(), columns.end(), i) == columns.end())
					returnVec.push_back(i);
			return returnVec;
		}
	}

	VeitchDiagram::VeitchDiagram(unsigned int nVariables) : nVariables(nVariables)
	{
		matrix = Matrix<bool>(GetHeight(), std::vector<bool>(GetWidth()));
	}

	// Helper function
	std::vector<unsigned int>
	VeitchDiagram::GetIntersection(
			const std::vector<unsigned int> &test, int i, const Implicant &implicant, bool rows) const
	{
		std::vector<unsigned int> returnVec;
		std::copy_if(test.begin(), test.end(), std::back_inserter(returnVec), [&](auto &x) {
			// Goes through the rows/columns of all other variables
			for (int j = i - 1; j >= 0; --j)
			{
				if (!implicant.Contains(nVariables - 1 - j))
					continue;
				// Rows/columns of the chosen variable
				auto other = rows ? GetVariableRows(j, !implicant[nVariables - 1 - j]) : GetVariableColumns(j,
																											!implicant[
																													nVariables -
																													1 -
																													j]);
				if (other.size() && std::find(other.begin(), other.end(), x) == other.end())
					// The chosen column has not been found in the new variable
					return false;
			}
			return true;
		});
		return returnVec;
	}

	VeitchDiagram::VeitchDiagram(unsigned int nVariables, const DNF &dnf) : VeitchDiagram(nVariables)
	{
		for (auto &x : dnf)
		{
			// Contain the rows and columns covered by this implicant, respectively
			std::vector<unsigned int> rows, columns;
			bool rowsDone = false, columnsDone = false;
			for (int i = nVariables - 1; i >= 0; --i)
				if (x.Contains(nVariables - 1 - i)) // Order of variables: E D C B A
				{
					auto testRows = GetVariableRows(i, !x[nVariables - 1 - i]);
					if (!rowsDone && testRows.size())
					{ // This variable is determined by a set of rows, find intersection of rows
						rowsDone = true;
						rows = GetIntersection(testRows, i, x, true);
						if (columnsDone)
							break;
						else continue;
					}
					auto testColumns = GetVariableColumns(i, !x[nVariables - 1 - i]);
					if (!columnsDone && testColumns.size())
					{ // This variable is determined by a set of columns, find intersection of columns
						columnsDone = true;
						columns = GetIntersection(testColumns, i, x, false);
						if (rowsDone)
							break;
						else continue;
					}
				}
			// If rows is empty, then the implicant does not contain any variables defined by rows
			if (rows.empty())
			{
				rows = std::vector<unsigned int>(GetHeight());
				for (int i = 0; i < rows.size(); ++i)
					rows[i] = i;
			}
			if (columns.empty())
			{
				columns = std::vector<unsigned int>(GetWidth());
				for (int i = 0; i < columns.size(); ++i)
					columns[i] = i;
			}
			for (auto i : rows)
				for (auto j : columns)
					matrix[i][j] = true;
		}
	}

	ullong VeitchDiagram::GetWidth() const
	{
		return 1ULL << ((nVariables + 1) / 2);
	}

	ullong VeitchDiagram::GetHeight() const
	{
		return 1ULL << (nVariables / 2);
	}

	unsigned int VeitchDiagram::GetNumberOfVariables() const
	{
		return nVariables;
	}

	Matrix<bool> VeitchDiagram::GetMatrixRepresentation() const
	{
		return matrix;
	}
}
