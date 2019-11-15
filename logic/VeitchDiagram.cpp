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

	VeitchDiagram::VeitchDiagram(unsigned int nVariables) : nVariables(nVariables)
	{
		matrix = Matrix<unsigned short>(GetHeight(), std::vector<unsigned short>(GetWidth()));
	}

	VeitchDiagram::VeitchDiagram(unsigned int nVariables, DNF dnf, const DNF &dontCare, bool shouldMinimize)
			: VeitchDiagram(nVariables)
	{
		if (shouldMinimize)
			dnf = *GetMDNF(nVariables, dnf, dontCare).begin();
		dnf.insert(dontCare.begin(), dontCare.end());
		for (auto &x : dnf)
		{
			unsigned short term = dontCare.find(x) != dontCare.end() ? 2 : 1;
			// Contains the rows and columns covered by this implicant
			Contour contour = GetContour(x);
			if (term == 1)
				contours.push_back(contour);
			for (auto i : contour.rows)
				for (auto j : contour.columns)
					matrix[i][j] = term;
		}
	}

	VeitchDiagram::VeitchDiagram(unsigned int nVariables, DNF dnf, bool shouldMinimize) : VeitchDiagram(nVariables, dnf,
																										{},
																										shouldMinimize)
	{
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

	Matrix<unsigned short> VeitchDiagram::GetMatrixRepresentation() const
	{
		return matrix;
	}

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
			for (int i = 0; i < GetWidth(); ++i)
				if (std::find(columns.begin(), columns.end(), i) == columns.end())
					returnVec.push_back(i);
			return returnVec;
		}
	}

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

	VeitchDiagram::Contour VeitchDiagram::GetContour(const Implicant &implicant) const
	{
		Contour contour;
		bool rowsDone = false, columnsDone = false;
		for (int i = nVariables - 1; i >= 0; --i)
			if (implicant.Contains(nVariables - 1 - i)) // Order of variables: E D C B A
			{
				auto testRows = GetVariableRows(i, !implicant[nVariables - 1 - i]);
				if (!rowsDone && testRows.size())
				{ // This variable is determined by a set of rows, find intersection of rows
					rowsDone = true;
					contour.rows = GetIntersection(testRows, i, implicant, true);
					if (columnsDone)
						break;
					else continue;
				}
				auto testColumns = GetVariableColumns(i, !implicant[nVariables - 1 - i]);
				if (!columnsDone && testColumns.size())
				{ // This variable is determined by a set of columns, find intersection of columns
					columnsDone = true;
					contour.columns = GetIntersection(testColumns, i, implicant, false);
					if (rowsDone)
						break;
					else continue;
				}
			}
		// If rows is empty, then the implicant does not contain any variables defined by rows
		if (contour.rows.empty())
		{
			contour.rows = std::vector<unsigned int>(GetHeight());
			for (int i = 0; i < contour.rows.size(); ++i)
				contour.rows[i] = i;
		}
		if (contour.columns.empty())
		{
			contour.columns = std::vector<unsigned int>(GetWidth());
			for (int i = 0; i < contour.columns.size(); ++i)
				contour.columns[i] = i;
		}
		return contour;
	}

	std::vector<VeitchDiagram::Contour> VeitchDiagram::GetContours() const
	{
		return contours;
	}

	bool Contains(const std::vector<unsigned int> &v, unsigned int x)
	{
		return std::find(v.begin(), v.end(), x) != v.end();
	}

	// Helper function
	void Put(char &slot, char newChar)
	{
		if (slot != ' ' && slot != newChar)
			slot = '+';
		else
			slot = newChar;
	}

	// Helper function
	void DrawContour(std::vector<std::string> &output, const VeitchDiagram::Contour &c, int width, int height)
	{
		int startX = 0, startY = 0;
		for (auto i : c.rows)
		{
			for (auto j : c.columns)
			{
				// x and y coordinates of the value character of the given field
				int centerX = startX + j * 8 + 4, centerY = startY + i * 4 + 2;
				// These determine whether there is a term above, below,... the term given by [i][j]
				bool above = Contains(c.rows, (i + height - 1) % height) && (c.rows.size() != height || i != 0),
						below = Contains(c.rows, (i + 1) % height) && (c.rows.size() != height || i != height - 1),
						left = Contains(c.columns, (j + width - 1) % width) && (c.columns.size() != width || j != 0),
						right = Contains(c.columns, (j + 1) % width) && (c.columns.size() != width || j != width - 1);
				// Top line
				if (!above) // There are no terms above this one
					for (int k = -2; k <= 2; ++k)
						Put(output[centerY - 1][centerX + k], '-');

				// Bottom line
				if (!below) // There are no terms above this one
					for (int k = -2; k <= 2; ++k)
						Put(output[centerY + 1][centerX + k], '-');

				// Right line
				if (!right)
					for (int k = -1; k <= 1; ++k)
						Put(output[centerY + k][centerX + 2], '|');

				// Left line
				if (!left)
					for (int k = -1; k <= 1; ++k)
						Put(output[centerY + k][centerX - 2], '|');
				/*
				 * Junctions (+ signs)
				 */
				// Above left
				if (!above && !left)
					output[centerY - 1][centerX - 2] = '+';
				// Above right
				if (!above && !right)
					output[centerY - 1][centerX + 2] = '+';
				// Below left
				if (!below && !left)
					output[centerY + 1][centerX - 2] = '+';
				// Below right
				if (!below && !right)
					output[centerY + 1][centerX + 2] = '+';
				/*
				 * Intersections with table lines
				 */
				// Right intersection
				if (right && j != width - 1)
				{
					if (!above)
						for (int k = 3; k <= 4; ++k)
							output[centerY - 1][centerX + k] = '-';
					if (!below)
						for (int k = 3; k <= 4; ++k)
							output[centerY + 1][centerX + k] = '-';
				}
				// Left intersection
				if (left && j != 0)
				{
					if (!above)
						for (int k = 3; k <= 4; ++k)
							output[centerY - 1][centerX - k] = '-';
					if (!below)
						for (int k = 3; k <= 4; ++k)
							output[centerY + 1][centerX - k] = '-';
				}
				// Above intersection
				if (above && i != 0)
				{
					if (!left)
						output[centerY - 2][centerX - 2] = '|';
					if (!right)
						output[centerY - 2][centerX + 2] = '|';
				}
			}
		}
	}

	std::ostream &operator<<(std::ostream &stream, const VeitchDiagram &diagram)
	{
		// TODO Change height & width
		std::vector<std::string> output(4 * diagram.GetHeight() + 1, std::string(8 * diagram.GetWidth() + 1, '0'));
		int startX = 0, startY = 0, endX = output[0].size(), endY = output.size();
		// Filling the output with the Veitch diagram before drawing any contours.
		for (int i = startY; i < endY; ++i)
			for (int j = startX; j < endX; ++j)
			{
				if (i % 4 == 0 && j % 8 != 0)
					output[i][j] = '-';
				else if (j % 8 == 0 && i % 4 != 0)
					output[i][j] = '|';
				else if (i % 4 == 0 && j % 4 == 0)
					output[i][j] = '+';
				else if (i % 4 == 2 && j % 8 == 4)
					output[i][j] =
							diagram.matrix[i / 4][j / 8] == 2 ? 'x' : '0' + diagram.matrix[i / 4][j / 8];
				else output[i][j] = ' ';
			}
		for (auto &contour : diagram.contours)
			DrawContour(output, contour, diagram.GetWidth(), diagram.GetHeight());
		// Sending the output to stream
		for (int i = 0; i < output.size(); ++i)
		{
			if (i != 0) stream << std::string{'\n'};
			stream << output[i];
		}
		return stream;
	}
}
