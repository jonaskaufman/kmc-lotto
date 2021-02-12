#ifndef RANDOM_H
#define RANDOM_H

#include <random>

namespace lotto
{
/**
 * Random number generator
 *
 * Allows sampling of random integers and reals.
 */
class RandomGenerator
{
public:
    /// Default constructor, automatically seeds generator
    RandomGenerator()
    {
        std::random_device device;
        reseed_generator(device());
    }

    /// Returns a random integer between 0 and maximum_value (inclusive)
    int sample_integer_range(int maximum_value)
    {
        return std::uniform_int_distribution<int>(0, maximum_value)(generator);
    }

    /// Returns a random real from the unit interval
    double sample_unit_interval() { return std::uniform_real_distribution<double>(0.0, 1.0)(generator); }

private:
    /// 64-bit Mersenne Twister generator
    std::mt19937_64 generator;

    /// Reseeds the generator
    void reseed_generator(unsigned int seed) { generator = std::mt19937_64(seed); }

    //    /// Uniform real distribution on unit interval
    //    std::uniform_real_distribution<double> uniform_unit_interval_distribution;
};
} // namespace lotto

#endif
