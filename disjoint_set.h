#pragma once

#include <cstddef>
#include <vector>

class DisjointSet
{
public:
    explicit DisjointSet(size_t size);

    size_t find(size_t value);
    size_t find(size_t value) const;
    bool connected(size_t left, size_t right);
    bool connected(size_t left, size_t right) const;
    void unite(size_t left, size_t right);

private:
    std::vector<size_t> parent_;
    std::vector<size_t> rank_;
};
