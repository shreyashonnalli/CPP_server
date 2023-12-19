#ifndef matrix_hpp
#define matrix_hpp

#include <stdio.h>
#include <iostream>
#include <boost/container_hash/hash.hpp>
#include <string>

constexpr uint32_t GRID_SIZE = std::numeric_limits<uint32_t>::max();

class SparseMatrix
{
public:
    SparseMatrix();

    // read string from sparse grid in a direction.
    std::string readFromGrid(uint32_t startRow, uint32_t startCol, std::string direction);

    // inserts a string into the grid. Called by a ClientHandler with a lock.
    int insertString(uint32_t startRow, uint32_t startCol, std::string direction, std::string toInsert);
    char get(uint32_t row, uint32_t col);
    void printMatrix() const;

    // retrieve Data Structure
    std::unordered_map<
        std::pair<uint32_t, uint32_t>,
        char,
        boost::hash<std::pair<uint32_t, uint32_t>>>
    getSparseMatrix();

    // remove all chars in matrix.
    void emptySparseMatrix();

    // To write to an output stream.
    friend std::ostream &operator<<(std::ostream &os, const SparseMatrix &matrix);

private:
    // The data structure.
    std::unordered_map<
        std::pair<uint32_t, uint32_t>,
        char,
        boost::hash<std::pair<uint32_t, uint32_t>>>
        matrix_;

    // helpers for the DS.
    std::pair<uint32_t, uint32_t> getKey(uint32_t row, uint32_t col);
    bool isInsertValid(uint32_t startRow, uint32_t startCol, std::string direction, u_long writeSize);
    bool isReadValid(uint32_t startRow, uint32_t startCol);
};

#endif /* matrix_hpp */
