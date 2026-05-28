#pragma once

#include "disjoint_set.h"

#include <cstddef>
#include <vector>

class Percolation
{
public:
    explicit Percolation(size_t dimension);

    void open(size_t row, size_t column);
    bool is_open(size_t row, size_t column) const;
    bool is_full(size_t row, size_t column) const;
    bool percolates() const;
    size_t get_number_of_open_cells() const;
    size_t dimension() const;

private:
    size_t index(size_t row, size_t column) const;
    void validate(size_t row, size_t column) const;
    void connect_if_open(size_t cell, bool neighbor_exists, size_t neighbor_row, size_t neighbor_column);

    size_t dimension_;
    std::vector<bool> open_;
    DisjointSet percolation_set_;
    DisjointSet fullness_set_;
    size_t virtual_top_;
    size_t virtual_bottom_;
    size_t open_count_;
};
