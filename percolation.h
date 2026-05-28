#pragma once

#include <cstddef>
#include <random>
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

struct PercolationStats
{
    PercolationStats(size_t dimension, size_t trials);

    double get_mean() const;
    double get_standard_deviation() const;
    double get_confidence_low() const;
    double get_confidence_high() const;
    void execute();

private:
    double run_trial(std::mt19937& generator) const;
    void calculate_statistics();

    size_t dimension_;
    size_t trials_;
    std::vector<double> thresholds_;
    double mean_;
    double standard_deviation_;
    double confidence_low_;
    double confidence_high_;
};
