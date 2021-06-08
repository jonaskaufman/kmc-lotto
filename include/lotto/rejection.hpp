#ifndef REJECTION_H
#define REJECTION_H

#include "event_selector.hpp"
#include <memory>
#include <stdexcept>
#include <vector>

class RejectionEventSelectorTest;

namespace lotto
{
/*
 * Event selector implemented using rejection KMC algorithm
 */
template <typename EventIDType, typename RateCalculatorType>
class RejectionEventSelector : public EventSelectorBase<EventIDType, RateCalculatorType>
{
public:
    RejectionEventSelector(const std::shared_ptr<RateCalculatorType>& rate_calculator_ptr,
                           double rate_upper_bound,
                           const std::vector<EventIDType>& event_id_list)
        : EventSelectorBase<EventIDType, RateCalculatorType>(rate_calculator_ptr),
          rate_upper_bound(rate_upper_bound),
          event_id_list(event_id_list)
    {
        // Make sure that provided parameters make sense
        if (rate_upper_bound <= 0.0)
        {
            throw std::runtime_error("Rate upper bound must be positive.");
        }
        if (event_id_list.empty())
        {
            throw std::runtime_error("Event ID list must not be empty.");
        }
    }

    // Attempts events, repeats until an event is accepted and returns the ID of selected event and accumulated time step
    std::pair<EventIDType, double> select_event()
    {
        EventIDType selected_event_id;
        double accumulated_time_step = 0;
        double total_rate = rate_upper_bound * event_id_list.size();
        // This could loop forever, but I don't think it makes sense to impose a limit or ask the user to do so
        // TODO: Make note in documentation about this
        // TODO: Give warning after a certain number of iterations (every N iterations)
        while (true)
        {
            accumulated_time_step += this->calculate_time_step(total_rate);
            EventIDType candidate_event_id = event_id_list[this->random_generator.sample_integer_range(event_id_list.size() - 1)];
            double rate = this->calculate_rate(candidate_event_id);
            assert(rate <= rate_upper_bound); // rate cannot exceed upper bound
            if (rate / rate_upper_bound >= this->random_generator.sample_unit_interval())
            {
                selected_event_id = candidate_event_id;
                break;
            }
        }
        return std::make_pair(selected_event_id, accumulated_time_step);
    }

private:
    // Upper bound on event rates
    const double rate_upper_bound;

    // List of IDs of all possible events
    const std::vector<EventIDType> event_id_list;

    // Friend for testing
    friend class ::RejectionEventSelectorTest;
};
} // namespace lotto
#endif
