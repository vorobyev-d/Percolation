#include "disjoint_set.h"

#include <algorithm>
#include <numeric>

DisjointSet::DisjointSet(size_t size) : parent_(size), rank_(size, 0)
{
    std::iota(parent_.begin(), parent_.end(), 0);
}

size_t DisjointSet::find(size_t value)
{
    if (parent_[value] != value)
    {
        parent_[value] = find(parent_[value]);
    }

    return parent_[value];
}

size_t DisjointSet::find(size_t value) const
{
    while (parent_[value] != value)
    {
        value = parent_[value];
    }

    return value;
}

bool DisjointSet::connected(size_t left, size_t right)
{
    return find(left) == find(right);
}

bool DisjointSet::connected(size_t left, size_t right) const
{
    return find(left) == find(right);
}

void DisjointSet::unite(size_t left, size_t right)
{
    size_t left_root = find(left);
    size_t right_root = find(right);

    if (left_root == right_root)
    {
        return;
    }

    if (rank_[left_root] < rank_[right_root])
    {
        std::swap(left_root, right_root);
    }

    parent_[right_root] = left_root;

    if (rank_[left_root] == rank_[right_root])
    {
        ++rank_[left_root];
    }
}
