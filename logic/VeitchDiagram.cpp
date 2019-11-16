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
			{{0, 1}, {}, {}, {1, 2}} //n=5
	};
	std::vector<unsigned int> columnLookupTable[5][5] = {
			// A	 B   C   D    E
			{}, // n=1
			{{}, {0}},  // n=2
			{{}, {0, 1},       {1, 2}}, // n=3
			{{}, {0, 1},       {1, 2}}, // n=4
			{{}, {0, 1, 4, 5}, {1, 2, 5, 6}, {}, {0, 1, 2, 3}} //n=5
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
					if (matrix[i][j] != 2)
						matrix[i][j] = term;
		}
	}

	VeitchDiagram::VeitchDiagram(unsigned int nVariables, DNF dnf, bool shouldMinimize) : VeitchDiagram(nVariables, dnf, {}, shouldMinimize)
	{
	}

	ullong VeitchDiagram::GetWidth() const
	{
		return nVariables == 1 ? 1 : 1ULL << ((nVariables + 1) / 2);
	}

	ullong VeitchDiagram::GetHeight() const
	{
		return nVariables == 1 ? 2 : 1ULL << (nVariables / 2);
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
				if (!implicant.Contains(nVariables - 1 - j)) // E D C B A
					continue;
				// Rows/columns of the chosen variable
				auto other = rows ? GetVariableRows(j, !implicant[nVariables - 1 - j]) : GetVariableColumns(j, !implicant[nVariables - 1 - j]);
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
	void DrawContour(std::vector<std::string> &output, const VeitchDiagram::Contour &c, int width, int height, int startX, int startY)
	{
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

	enum Pos
	{
		ABOVE, BELOW, LEFT, RIGHT
	};

	// Helper function, returns the drawing position of a variable in the diagram
	Pos GetPosition(unsigned int index)
	{
		switch (index)
		{
			case 0: // A
				return LEFT;
			case 1: // B
				return ABOVE;
			case 2: // C
				return BELOW;
			case 3: // D
				return RIGHT;
			case 4: // E
				return BELOW;
		}
		return LEFT;
	}

	// Helper function
	std::vector<std::vector<unsigned int>> ContiguousPartition(const std::vector<unsigned int> &v)
	{
		std::vector<std::vector<unsigned int>> returnVec;
		for (int i = 0; i < v.size(); ++i)
		{
			std::vector<unsigned int> temp{v[i]};
			int j = i + 1;
			for (; j < v.size() && v[j] - v[j - 1] == 1; ++j)
				temp.push_back(v[j]);
			returnVec.push_back(temp);
			i = j - 1;
		}
		return returnVec;

	}

	std::ostream &VeitchDiagram::Print(std::ostream &stream, bool drawBorders) const
	{
		// Holds the numbers of variables that are drawn above, below, left and right of the table, respectively.
		unsigned char nPos[4]{};
		for (int i = 0; i < nVariables; ++i)
			++nPos[GetPosition(i)];
		int startX = 4 * nPos[LEFT], startY = 2 * nPos[ABOVE],
				endPaddingX = 4 * nPos[RIGHT], endPaddingY = 2 * nPos[BELOW];
		std::vector<std::string> output(startY + 4 * GetHeight() + 1 + endPaddingY,
										std::string(startX + 8 * GetWidth() + 1 + endPaddingX, ' '));
		int endX = output[0].size() - endPaddingX, endY = output.size() - endPaddingY;
		for (int i = nVariables - 1; i >= 0; --i)
		{
			Pos pos = GetPosition(i);
			if (pos == LEFT)
			{
				auto allRows = ContiguousPartition(GetVariableRows(i, false));
				for (auto &rows : allRows)
				{
					for (unsigned int row : rows)
						for (int k = 1; k <= 3; ++k)
							output[startY + 4 * row + k][startX - 4 * nPos[LEFT] + 2] = '|';
					output[startY + 2 * (rows[0] + rows[rows.size() - 1]) + 2][startX - 4 * nPos[LEFT]] = 'A' + i;
				}
			}
			else if (pos == ABOVE)
			{
				auto allColumns = ContiguousPartition(GetVariableColumns(i, false));
				for (auto &columns : allColumns)
				{
					for (unsigned int column : columns)
						for (int k = 1; k <= 7; ++k)
							output[startY - 2 * nPos[ABOVE] + 1][startX + 8 * column + k] = '-';
					output[startY - 2 * nPos[ABOVE]][startX + 4 * (columns[0] + columns[columns.size() - 1]) + 4] = 'A' + i;
				}
			}
			else if (pos == BELOW)
			{
				auto allColumns = ContiguousPartition(GetVariableColumns(i, false));
				for (auto &columns : allColumns)
				{
					for (unsigned int column : columns)
						for (int k = 1; k <= 7; ++k)
							output[endY + 2 * nPos[BELOW] - 2][startX + 8 * column + k] = '-';
					output[endY + 2 * nPos[BELOW] - 1][startX + 4 * (columns[0] + columns[columns.size() - 1]) + 4] = 'A' + i;
				}
			}
			else if (pos == RIGHT)
			{
				auto allRows = ContiguousPartition(GetVariableRows(i, false));
				for (auto &rows : allRows)
				{
					for (unsigned int row : rows)
						for (int k = 1; k <= 3; ++k)
							output[startY + 4 * row + k][endX + 4 * nPos[RIGHT] - 3] = '|';
					output[startY + 2 * (rows[0] + rows[rows.size() - 1]) + 2].at(endX + 4 * nPos[RIGHT] - 1) = 'A' + i;
				}
			}
			--nPos[pos];
		}
		// Filling the output with the Veitch diagram before drawing any contours.
		for (int i = startY; i < endY; ++i)
			for (int j = startX; j < endX; ++j)
			{
				if (drawBorders)
				{
					if ((i - startY) % 4 == 0 && (j - startX) % 8 != 0)
						output[i][j] = '-';
					else if ((j - startX) % 8 == 0 && (i - startY) % 4 != 0)
						output[i][j] = '|';
					else if ((i - startY) % 4 == 0 && (j - startX) % 4 == 0)
						output[i][j] = '+';
				}
				if ((i - startY) % 4 == 2 && (j - startX) % 8 == 4)
					output[i][j] =
							matrix[(i - startY) / 4][(j - startX) / 8] == 2
							? 'x' : '0' + matrix[(i - startY) / 4][(j - startX) / 8];
			}
		// Drawing contours
		for (auto &contour : contours)
			DrawContour(output, contour, GetWidth(), GetHeight(), startX, startY);
		// Sending output to stream
		for (int i = 0; i < output.size(); ++i)
		{
			if (i != 0) stream << std::string{'\n'};
			stream << output[i];
		}
		return stream;
	}

	std::ostream &operator<<(std::ostream &stream, const VeitchDiagram &diagram)
	{
		return diagram.Print(stream);
	}
}
