#include "test_parameters.hpp"
#include "statistics.hpp"
#include <algorithm>
#include <gtest/gtest.h>
#include <lotto/random.hpp>
#include <vector>

class RandomGeneratorTest : public testing::Test
{
protected:
    /// Random generator for testing
    lotto::RandomGenerator generator;

    /// Checks that samples satisfy bounds and expected mean for a uniform
    //  distribution between min_value and max_value
    template <typename T>
    void check_samples_from_uniform_distribution(T min_value, T max_value, std::vector<T> samples)
    {
        // Check min and max
        T max_sample = *std::max_element(samples.begin(), samples.end());
        T min_sample = *std::min_element(samples.begin(), samples.end());
        EXPECT_GE(min_sample, min_value);
        EXPECT_LE(max_sample, max_value);

        // Check mean
        double mean = get_mean(samples);
        double expected_mean = (double)(min_value + max_value) / 2.0;
        double sigma_of_mean =
            standard_error_of_mean(standard_deviation_of_uniform_distribution(min_value, max_value), samples.size());
        // Check if mean of samples is within TEST_SIGMA standard_deviations of expected value
        check_deviation_of_mean(mean, expected_mean, sigma_of_mean, TEST_SIGMA);
    }
};

TEST_F(RandomGeneratorTest, Construct)
{
    // Checks if RandomGenerator can be constructed
}

TEST_F(RandomGeneratorTest, GetSeed)
{
    // Checks if get_seed returns the correct value
    for (lotto::UIntType i = 0; i < 100; ++i)
    {
        generator.reseed_generator(i);
        EXPECT_EQ(generator.get_seed(), i);
    }
}

TEST_F(RandomGeneratorTest, DefaultSeedNotFixed)
{
    // Checks that the default seed is not fixed
    lotto::UIntType first_seed = generator.get_seed();
    bool is_seed_different = false;
    int n_attempts = 100;
    for (int i = 0; i < n_attempts; ++i)
    {
        lotto::RandomGenerator new_generator;
        if (new_generator.get_seed() != first_seed)
        {
            is_seed_different = true;
            break;
        }
    }
    EXPECT_TRUE(is_seed_different) << "Seed value is unchanged after " << n_attempts
                                   << " attempts, random_device implementation appears to be deterministic";
}

TEST_F(RandomGeneratorTest, IntegerRangeSamples)
{
    // Checks that values from sample_integer_range behave as expected
    generator.reseed_generator(TEST_SEED); // fixed seed for testing
    lotto::UIntType min_value = 0;
    lotto::UIntType max_value = 1000;
    int n_samples = 100000000;
    std::vector<lotto::UIntType> samples(n_samples);
    for (int i = 0; i < n_samples; ++i)
    {
        samples[i] = generator.sample_integer_range(max_value);
    }
    check_samples_from_uniform_distribution(min_value, max_value, samples);
}

TEST_F(RandomGeneratorTest, UnitIntervalSamples)
{
    // Checks that values from sample_unit_interval behave as expected
    generator.reseed_generator(TEST_SEED); // fixed seed for testing
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
