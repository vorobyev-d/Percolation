#include "percolation.h"

#include <stdexcept>

Percolation::Percolation(size_t dimension) : dimension_(dimension), open_(dimension * dimension, false), percolation_set_(dimension * dimension + 2),
    fullness_set_(dimension * dimension + 1), virtual_top_(dimension * dimension), virtual_bottom_(dimension * dimension + 1), open_count_(0)
{
    if (dimension == 0)
    {
        throw std::invalid_argument("percolation grid dimension must be positive");
    }
}

void Percolation::open(size_t row, size_t column)
{
    validate(row, column);

    const size_t cell = index(row, column);
    if (open_[cell])
    {
        return;
    }

    open_[cell] = true;
    ++open_count_;

    if (row == 0)
    {
        percolation_set_.unite(cell, virtual_top_);
        fullness_set_.unite(cell, virtual_top_);
    }

    if (row + 1 == dimension_)
    {
        percolation_set_.unite(cell, virtual_bottom_);
    }

    connect_if_open(cell, row > 0, row - 1, column);
    connect_if_open(cell, row + 1 < dimension_, row + 1, column);
    connect_if_open(cell, column > 0, row, column - 1);
    connect_if_open(cell, column + 1 < dimension_, row, column + 1);
}

bool Percolation::is_open(size_t row, size_t column) const
{
    validate(row, column);
    return open_[index(row, column)];
}

bool Percolation::is_full(size_t row, size_t column) const
{
    validate(row, column);
    const size_t cell = index(row, column);
    return open_[cell] && fullness_set_.connected(cell, virtual_top_);
}

bool Percolation::percolates() const
{
    return percolation_set_.connected(virtual_top_, virtual_bottom_);
}

size_t Percolation::get_number_of_open_cells() const
{
    return open_count_;
}

size_t Percolation::dimension() const
{
    return dimension_;
}

size_t Percolation::index(size_t row, size_t column) const
{
    return row * dimension_ + column;
}

void Percolation::validate(size_t row, size_t column) const
{
    if (row >= dimension_ || column >= dimension_)
    {
        throw std::out_of_range("percolation cell index is out of range");
    }
}

void Percolation::connect_if_open(size_t cell, bool neighbor_exists, size_t neighbor_row, size_t neighbor_column)
{
    if (!neighbor_exists)
    {
        return;
    }

    const size_t neighbor = index(neighbor_row, neighbor_column);
    if (!open_[neighbor])
    {
        return;
    }

    percolation_set_.unite(cell, neighbor);
    fullness_set_.unite(cell, neighbor);
}
