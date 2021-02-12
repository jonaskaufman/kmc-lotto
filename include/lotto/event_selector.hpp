#ifndef EVENT_SELECTOR_H
#define EVENT_SELECTOR_H

#include "random.hpp"
#include <utility>

namespace lotto
{
/*
 * Base class template for event selector
 */
template <typename EventIDType, typename RateCalculatorType>
class EventSelectorBase
{
public:
    EventSelectorBase() = delete;

    // Selects a single event, returns the event ID and the time step
    virtual std::pair<EventIDType, double> select_event() = 0;

protected:
    // Pointer to rate calculator
    const RateCalculatorType* rate_calculator;

    // Random number generator
    RandomGenerator random_generator;

    // Constructor for use in derived classes
    EventSelectorBase(RateCalculatorType* rate_calculator) : rate_calculator(rate_calculator) {}

    // Returns the rate given an event ID
    double calculate_rate(const EventIDType& event_id) { return rate_calculator->calculate_rate(event_id); }

    // Returns the time step for Poisson process, given the total rate
    double calculate_time_step(double total_rate)
    {
        // TODO: Add checks for log/div of zero
        double time_step = -std::log(random_generator.sample_unit_interval()) / total_rate;
        return time_step;
    }
};
} // namespace lotto
#endif
