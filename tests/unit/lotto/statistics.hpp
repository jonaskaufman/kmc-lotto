#ifndef STATISTICS_H
#define STATISTICS_H

#include <cmath>
#include <gtest/gtest.h>
#include <numeric>
#include <vector>

/// Returns the mean of a list of values
template <typename T>
double get_mean(std::vector<T> values)
{
    double sum = std::accumulate(values.begin(), values.end(), 0.0);
    double mean = sum / values.size();
    return mean;
}

/// Returns the standard error of the mean given the sample standard deviation and the number of samples
inline double standard_error_of_mean(double sample_standard_deviation, int n_samples)
{
    return sample_standard_deviation / std::sqrt((double)n_samples);
}

/// Returns the standard deviation of a (continous) uniform distrubtion between min_value and max_value
inline double standard_deviation_of_uniform_distribution(double min_value, double max_value)
{
    return (max_value - min_value) / std::sqrt((double)12);
}

/// Checks if the mean deviates from its expected value by more than sigma_tolerance times the standard error
inline void
check_deviation_of_mean(double mean, double expected_mean, double standard_error_of_mean, double sigma_tolerance)
{
    EXPECT_LE(std::abs(mean - expected_mean), sigma_tolerance * standard_error_of_mean);
}

#endif
