#ifndef STATISTICS_H
#define STATISTICS_H

#include "test_parameters.hpp"
#include <algorithm>
#include <cmath>
#include <gtest/gtest.h>
#include <numeric>
#include <vector>

/// Returns the mean of a list of values
template <typename T>
double mean(std::vector<T> values)
{
    double sum = std::accumulate(values.begin(), values.end(), 0.0);
    double mean = sum / values.size();
    return mean;
}

/// Returns the standard error of the mean given the standard deviation and the number of samples
inline double standard_error_of_mean(double standard_deviation, int n_samples)
{
    return standard_deviation / std::sqrt((double)n_samples);
}

/// Checks if the sample mean deviates from its true value by more than sigma_tolerance times the standard error
inline void
check_deviation_of_mean(double sample_mean, double true_mean, double standard_error_of_mean, double sigma_tolerance)
{
    EXPECT_LE(std::abs(sample_mean - true_mean), sigma_tolerance * standard_error_of_mean);
    return;
}

/// Checks that samples satisfy bounds and expected mean for a uniform
//  distribution between min_value and max_value
template <typename T>
void check_samples_from_uniform_distribution(T min_value, T max_value, std::vector<T> samples)
{
    // Check min and max
    ASSERT_GT(max_value, min_value);
    T max_sample = *std::max_element(samples.begin(), samples.end());
    T min_sample = *std::min_element(samples.begin(), samples.end());
    EXPECT_GE(min_sample, min_value);
    EXPECT_LE(max_sample, max_value);

    // Check mean
    double true_mean = (double)(min_value + max_value) / 2.0;
    double true_standard_deviation = (double)(max_value - min_value) / std::sqrt(12.0);
    double sample_mean = mean(samples);
    double standard_error = standard_error_of_mean(true_standard_deviation, samples.size());
    check_deviation_of_mean(sample_mean, true_mean, standard_error, TEST_SIGMA);
}

/// Checks that samples satisfy bounds and expected mean for an inverse log distribution
//  a*ln(1/x), where x is uniformly distributed between 0 and 1,
//  which has mean and standard deviation a
void check_samples_from_log_inverse_distribution(double a, std::vector<double> samples)
{
    // Check min (a must be positive)
    ASSERT_GT(a, 0.0);
    double min_sample = *std::min_element(samples.begin(), samples.end());
    EXPECT_GE(min_sample, 0.0);

    // Check mean
    double true_mean = a;
    double true_standard_deviation = a;
    double sample_mean = mean(samples);
    double standard_error = standard_error_of_mean(true_standard_deviation, samples.size());
    check_deviation_of_mean(sample_mean, true_mean, standard_error, TEST_SIGMA);
}

#endif
