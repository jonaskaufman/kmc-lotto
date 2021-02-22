#include <algorithm>
#include <cmath>
#include <gtest/gtest.h>
#include <lotto/random.hpp>
#include <numeric>
#include <vector>

class RandomGeneratorTest : public testing::Test
{
protected:
    /// Random generator for testing
    lotto::RandomGenerator generator;

    /// Returns the expected standard error of the mean
    //  for a uniform distribution between min and max
    double expected_standard_error_of_mean(double min, double max, int n_samples)
    {
        return (max - min) / std::sqrt(12) / std::sqrt(n_samples);
    }

    /// Returns the average of a set of values
    template <typename T>
    double get_average(std::vector<T> values)
    {
        double sum = std::accumulate(values.begin(), values.end(), 0.0);
        double average = sum / (double)values.size();
        return average;
    }

    /// Checks that samples satisfy bounds and expected average for a uniform
    //  distribution between min_value and max_value
    template <typename T>
    void check_samples_from_uniform_distribution(T min_value, T max_value, std::vector<T> samples)
    {
        // Check min and max
        T max_sample = *std::max_element(samples.begin(), samples.end());
        T min_sample = *std::min_element(samples.begin(), samples.end());
        ASSERT_TRUE(min_sample >= min_value);
        ASSERT_TRUE(max_sample <= max_value);

        // Check average
        double average = get_average(samples);
        double expected_average = (double)(min_value + max_value) / 2;
        double sigma_of_mean = expected_standard_error_of_mean(min_value, max_value, samples.size());
        // Average of samples should be *well* within 10 sigma of expected value
        ASSERT_TRUE(std::abs(average - expected_average) < 10 * sigma_of_mean);
    }
};

TEST_F(RandomGeneratorTest, Construct)
{
    // Checks if RandomGenerator can be constructed
    ASSERT_TRUE(true);
}

TEST_F(RandomGeneratorTest, GetSeed)
{
    // Checks if get_seed returns the correct value
    for (UIntType i = 0; i < 100; ++i)
    {
        generator.reseed_generator(i);
        ASSERT_TRUE(generator.get_seed() == i);
    }
}

TEST_F(RandomGeneratorTest, DefaultSeedNotFixed)
{
    // Checks that the default seed is not fixed
    UIntType first_seed = generator.get_seed();
    bool is_seed_different = false;
    for (int i = 0; i < 100; ++i)
    {
        lotto::RandomGenerator new_generator;
        if (new_generator.get_seed() != first_seed)
        {
            is_seed_different = true;
            break;
        }
    }
    ASSERT_TRUE(is_seed_different);
}

TEST_F(RandomGeneratorTest, IntegerRangeSamples)
{
    // Checks that values from sample_integer_range behave as expected
    generator.reseed_generator(0); // fixed seed for testing
    UIntType min_value = 0;
    UIntType max_value = 1000;
    int n_samples = 100000000;
    std::vector<UIntType> samples(n_samples);
    for (int i = 0; i < n_samples; ++i)
    {
        samples[i] = generator.sample_integer_range(max_value);
    }
    check_samples_from_uniform_distribution(min_value, max_value, samples);
}

TEST_F(RandomGeneratorTest, UnitIntervalSamples)
{
    // Checks that values from sample_unit_interval behave as expected
    generator.reseed_generator(0); // fixed seed for testing
    double min_value = 0.0;
    double max_value = 1.0;
    int n_samples = 100000000;
    std::vector<double> samples(n_samples);
    for (int i = 0; i < n_samples; ++i)
    {
        samples[i] = generator.sample_unit_interval();
    }
    check_samples_from_uniform_distribution(min_value, max_value, samples);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
