#ifndef REJECTION_H
#define REJECTION_H

#include "event_selector.hpp"
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
    RejectionEventSelector(RateCalculatorType* rate_calculator,
                           double rate_upper_bound,
                           const std::vector<EventIDType>& event_id_list)
        : EventSelectorBase<EventIDType, RateCalculatorType>(rate_calculator),
          rate_upper_bound(rate_upper_bound),
          event_id_list(event_id_list)
    {
    // TODO: add check for upper bound equaling zero
    }

    // Attempts events, repeats until an event is accepted and returns the ID of selected event and total time step
    std::pair<EventIDType, double> select_event()
    {
        EventIDType selected_event_id;
        double total_time_step = 0;
        double total_rate = rate_upper_bound * event_id_list.size();
        // TODO: add check to avoid infinite looping
        while (true)
        {
            total_time_step += this->calculate_time_step(total_rate);
            int candidate_event_id = this->random_generator.sample_integer_range(event_id_list.size() - 1);
            double rate = this->calculate_rate(candidate_event_id);
            if (rate / rate_upper_bound > this->random_generator.sample_unit_interval())
            {
                selected_event_id = candidate_event_id;
                break;
            }
        }
        return std::make_pair(selected_event_id, total_time_step);
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
