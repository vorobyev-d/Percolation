#include "percolation.h"

#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>
#include <stdexcept>

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

PercolationStats::PercolationStats(size_t dimension, size_t trials) : dimension_(dimension), trials_(trials), thresholds_(), mean_(0.0), 
                                                                    standard_deviation_(0.0), confidence_low_(0.0), confidence_high_(0.0)
{
    if (dimension == 0)
    {
        throw std::invalid_argument("percolation grid dimension must be positive");
    }

    if (trials == 0)
    {
        throw std::invalid_argument("amount of trials must be positive");
    }
}

double PercolationStats::get_mean() const
{
    return mean_;
}

double PercolationStats::get_standard_deviation() const
{
    return standard_deviation_;
}

double PercolationStats::get_confidence_low() const
{
    return confidence_low_;
}

double PercolationStats::get_confidence_high() const
{
    return confidence_high_;
}

void PercolationStats::execute()
{
    thresholds_.clear();
    thresholds_.reserve(trials_);

    std::random_device random_device;
    std::mt19937 generator(random_device());

    for (size_t trial = 0; trial < trials_; ++trial)
    {
        thresholds_.push_back(run_trial(generator));
    }

    calculate_statistics();
}

double PercolationStats::run_trial(std::mt19937& generator) const
{
    Percolation percolation(dimension_);
    std::vector<size_t> blocked_cells(dimension_ * dimension_);
    std::iota(blocked_cells.begin(), blocked_cells.end(), 0);

    while (!percolation.percolates())
    {
        std::uniform_int_distribution<size_t> distribution(0, blocked_cells.size() - 1);
        const size_t selected_position = distribution(generator);
        const size_t selected_cell = blocked_cells[selected_position];

        blocked_cells[selected_position] = blocked_cells.back();
        blocked_cells.pop_back();

        percolation.open(selected_cell / dimension_, selected_cell % dimension_);
    }

    return static_cast<double>(percolation.get_number_of_open_cells()) /
           static_cast<double>(dimension_ * dimension_);
}

void PercolationStats::calculate_statistics()
{
    const double sum = std::accumulate(thresholds_.begin(), thresholds_.end(), 0.0);
    mean_ = sum / static_cast<double>(trials_);

    if (trials_ == 1)
    {
        standard_deviation_ = 0.0;
    }
    else
    {
        double squared_difference_sum = 0.0;
        for (double threshold : thresholds_)
        {
            const double difference = threshold - mean_;
            squared_difference_sum += difference * difference;
        }

        standard_deviation_ = std::sqrt(squared_difference_sum / static_cast<double>(trials_ - 1));
    }

    const double confidence_delta =
        1.96 * standard_deviation_ / std::sqrt(static_cast<double>(trials_));

    confidence_low_ = mean_ - confidence_delta;
    confidence_high_ = mean_ + confidence_delta;
}
