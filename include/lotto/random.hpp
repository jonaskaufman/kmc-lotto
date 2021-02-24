#ifndef RANDOM_H
#define RANDOM_H

#include <cmath>
#include <cstdint>
#include <limits>
#include <random>

using UIntType = std::uint_fast64_t;
using RealType = double;

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
        : // std::uniform_real_distribution provides the interval [a, b)
          // so to obtain (a, b] we must shift the endpoints to the next representable values
          unit_interval_distribution(std::nextafter(0.0, std::numeric_limits<RealType>::max()),
                                     std::nextafter(1.0, std::numeric_limits<RealType>::max()))
    {
        std::random_device device;
        reseed_generator(device());
    }

    /// Returns a random integer from the closed interval [0, maximum_value]
    UIntType sample_integer_range(UIntType maximum_value)
    {
        return std::uniform_int_distribution<UIntType>(0, maximum_value)(generator);
    }

    /// Returns a random real from the half-open unit interval (0, 1]
    RealType sample_unit_interval() { return unit_interval_distribution(generator); }

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

    /// Half-open unit interval distribution (0, 1]
    std::uniform_real_distribution<RealType> unit_interval_distribution;
};
} // namespace lotto

#endif
