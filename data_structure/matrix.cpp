#include "matrix.hpp"
SparseMatrix::SparseMatrix() {}

// print matrix to console.
void SparseMatrix::printMatrix() const
{
    std::cout << "Size of the Matrix is: " << matrix_.size() << "\n\n";
    for (auto pair : matrix_)
    {
        std::cout << "(" << pair.first.first << "," << pair.first.second << ")"
                  << " = " << pair.second << "\n";
    }
}

// same as above but writes to an output stream.
std::ostream &operator<<(std::ostream &os, const SparseMatrix &matrix)
{
    os << "Size of the Matrix is: " << matrix.matrix_.size() << "\n\n";
    for (const auto &pair : matrix.matrix_)
    {
        os << "(" << pair.first.first << "," << pair.first.second << ")"
           << " = " << pair.second << "\n";
    }
    return os;
}

char SparseMatrix::get(uint32_t row, uint32_t col)
{
    auto key = getKey(row, col);
    if (!matrix_.count(key))
        return '\0';
    else
        return matrix_[key];
}

// makes sure a read to the DS is valid.
bool SparseMatrix::isReadValid(uint32_t startRow, uint32_t startCol)
{
    if (startRow < GRID_SIZE && startCol < GRID_SIZE)
        return true;
    else
        return false;
}

// Reads something only if it is valid.
std::string SparseMatrix::readFromGrid(uint32_t startRow, uint32_t startCol, std::string direction)
{
    std::string toReturn = "";
    if (!isReadValid(startRow, startCol))
    {
        std::cout << "not valid!\n";
        return toReturn;
    }

    uint32_t colPtr = startCol;
    uint32_t rowPtr = startRow;

    if (direction == "+X")
    {
        while (colPtr < GRID_SIZE && matrix_.count(getKey(startRow, colPtr)))
        {
            toReturn += matrix_[getKey(startRow, colPtr)];
            colPtr++;
        }
    }
    else if (direction == "-X")
    {
        while (matrix_.count(getKey(startRow, colPtr)))
        {
            toReturn += matrix_[getKey(startRow, colPtr)];
            if (colPtr == 0)
                break;
            colPtr--;
        }
    }
    else if (direction == "+Y")
    {
        while (rowPtr < GRID_SIZE && matrix_.count(getKey(rowPtr, startCol)))
        {
            toReturn += matrix_[getKey(rowPtr, startCol)];
            rowPtr++;
        }
    }
    else if (direction == "-Y")
    {
        while (matrix_.count(getKey(rowPtr, startCol)))
        {
            toReturn += matrix_[getKey(rowPtr, startCol)];
            if (rowPtr == 0)
                break;
            rowPtr--;
        }
    }
    return toReturn;
}

// finds out if insert is valid and doesn't go out of bounds.
bool SparseMatrix::isInsertValid(uint32_t startRow, uint32_t startCol, std::string direction, u_long writeSize)
{
    if (startRow == GRID_SIZE || startCol == GRID_SIZE)
        return false;

    long long resultingColPtr = static_cast<long long>(startCol);
    long long resultingRowPtr = static_cast<long long>(startRow);
    long long gridBound = static_cast<long long>(GRID_SIZE);

    if (direction == "+X")
        resultingColPtr = static_cast<long long>(startCol) + static_cast<long long>(writeSize);
    else if (direction == "-X")
        resultingColPtr = static_cast<long long>(startCol) - static_cast<long long>(writeSize);
    else if (direction == "+Y")
        resultingRowPtr = static_cast<long long>(startRow) + static_cast<long long>(writeSize);
    else if (direction == "-Y")
        resultingRowPtr = static_cast<long long>(startRow) - static_cast<long long>(writeSize);
    else
        return false;

    if (resultingColPtr >= -1 && resultingColPtr <= gridBound && resultingRowPtr >= -1 && resultingRowPtr <= gridBound)
        return true;
    return false;
}

// only writes if insert is valid. Returns int indicating status of insert.
int SparseMatrix::insertString(uint32_t startRow, uint32_t startCol, std::string direction, std::string toInsert)
{
    if (!isInsertValid(startRow, startCol, direction, toInsert.length()))
        return -1;
    u_long stringLength = toInsert.length();

    if (direction == "+X")
    {
        for (uint32_t i = startCol; i < startCol + stringLength; i++)
        {
            matrix_[getKey(startRow, i)] = toInsert[i - startCol];
        }
    }
    else if (direction == "-X")
    {
        long long endCondition = static_cast<long long>(startCol) - static_cast<long long>(stringLength);
        uint32_t strPtr = 0;
        for (uint32_t i = startCol; i > endCondition; i--)
        {
            matrix_[getKey(startRow, i)] = toInsert[strPtr];
            strPtr++;
            if (i == 0)
                break;
        }
    }
    else if (direction == "+Y")
    {
        for (uint32_t i = startRow; i < startRow + stringLength; i++)
        {
            matrix_[getKey(i, startCol)] = toInsert[i - startRow];
        }
    }
    else if (direction == "-Y")
    {
        long long endCondition = static_cast<long long>(startRow) - static_cast<long long>(stringLength);
        uint32_t strPtr = 0;
        for (uint32_t i = startRow; i > endCondition; i--)
        {
            matrix_[getKey(i, startCol)] = toInsert[strPtr];
            strPtr++;
            if (i == 0)
                break;
        }
    }

    return 1;
}

// get key of a row,col combination.
std::pair<uint32_t, uint32_t> SparseMatrix::getKey(uint32_t row, uint32_t col)
{
    return std::pair<uint32_t, uint32_t>{row, col};
}

// returns the Data Structure.
std::unordered_map<std::pair<uint32_t, uint32_t>, char, boost::hash<std::pair<uint32_t, uint32_t>>> SparseMatrix::getSparseMatrix()
{
    return matrix_;
}

// clear all values in map.
void SparseMatrix::emptySparseMatrix()
{
    matrix_.clear();
}
