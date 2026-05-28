#pragma once

#include <cstddef>
#include <random>
#include <vector>

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
