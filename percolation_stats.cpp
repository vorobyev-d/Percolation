#include "percolation_stats.h"

#include "percolation.h"

#include <cmath>
#include <numeric>
#include <random>
#include <stdexcept>

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
