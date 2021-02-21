#ifndef RANDOM_H
#define RANDOM_H

#include <cstdint>
#include <random>

using UIntType = std::uint_fast64_t;

namespace lotto
{
/**
 * Random number generator
 *
 * Allows sampling of random integers and reals using
 * the Mersenne Twister 19937 generator (64-bit)
 */
class RandomGenerator
{

public:
    /// Default constructor, automatically seeds generator from random device
    RandomGenerator()
    {
        std::random_device device;
        reseed_generator(device());
    }

    /// Returns a random integer between 0 and maximum_value (inclusive)
    UIntType sample_integer_range(UIntType maximum_value)
    {
        return std::uniform_int_distribution<UIntType>(0, maximum_value)(generator);
    }

    /// Returns a random real from the unit interval (includes 0, excludes 1)
    double sample_unit_interval() { return std::uniform_real_distribution<double>(0.0, 1.0)(generator); }

    /// Returns the value used to seed the generator
    UIntType get_seed() const { return seed; }

    /// Reseeds the generator
    void reseed_generator(UIntType new_seed)
    {
        seed = new_seed;
        generator.seed(seed);
    }

private:
    /// 64-bit Mersenne Twister generator
    std::mt19937_64 generator;

    /// Seed used for generator
    UIntType seed;

    // TODO: Store distributions as members?
};
} // namespace lotto

#endif
