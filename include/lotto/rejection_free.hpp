#ifndef REJECTION_FREE_H
#define REJECTION_FREE_H

#include "event_rate_tree.hpp"
#include "event_rate_tree_impl.hpp"
#include "event_selector.hpp"
#include <map>
#include <memory>
#include <vector>

class RejectionFreeEventSelectorTest;

namespace lotto
{

/*
 * Event selector implemented using rejection-free KMC algorithm
 */
template <typename EventIDType, typename RateCalculatorType>
class RejectionFreeEventSelector : public EventSelectorBase<EventIDType, RateCalculatorType>
{
public:
    // Construct given a rate calculator, event ID list, and impact table
    RejectionFreeEventSelector(const std::shared_ptr<RateCalculatorType>& rate_calculator_ptr,
                               const std::vector<EventIDType>& event_id_list,
                               const std::map<EventIDType, std::vector<EventIDType>>& impact_table)
        : EventSelectorBase<EventIDType, RateCalculatorType>(rate_calculator_ptr),
          event_rate_tree(event_id_list, this->calculate_rates(event_id_list)),
          impact_table(impact_table),
          impacted_events_ptr(nullptr)
    {
    }

    // Select an event and return its ID and the time step
    std::pair<EventIDType, double> select_event()
    {
        // Because this function only selects events and does not process them,
        // it cannot update any rates impacted by the selected event until the next call.
        update_impacted_event_rates();

        // Rates should now be updated. Calculate total rate and time step
        double total_rate = event_rate_tree.total_rate();
        double time_step = this->calculate_time_step(total_rate);

        // Query tree to select event
        double query_value = total_rate * this->random_generator.sample_unit_interval();
        EventIDType selected_event_id = event_rate_tree.query_tree(query_value);

        // Update impacted event list and return
        set_impacted_events(selected_event_id);
        return std::make_pair(selected_event_id, time_step);
    }

private:
    // Tree storing event IDs and their corresponding rates
    EventRateTree<EventIDType> event_rate_tree;

    // Lookup table indicating, for a given event that is accepted, which events' rates are impacted
    const std::map<EventIDType, std::vector<EventIDType>> impact_table;

    // Pointer to vector of impacted events whose rates have not been updated
    mutable const std::vector<EventIDType>* impacted_events_ptr;

    // Set the impact events pointer based on an accepted event ID
    void set_impacted_events(const EventIDType& accepted_event_id)
    {
        // TODO: Check that ptr is null?
        impacted_events_ptr = &impact_table.at(accepted_event_id);
        return;
    }

    // Update the stored rates for impacted events
    void update_impacted_event_rates()
    {
        if (impacted_events_ptr != nullptr)
        {
            for (const EventIDType& event_id : *impacted_events_ptr)
            {
                event_rate_tree.update_rate(event_id, this->calculate_rate(event_id));
            }
            impacted_events_ptr = nullptr;
        }
        return;
    }

    // Friend for testing
    friend class ::RejectionFreeEventSelectorTest;
};
} // namespace lotto

#endif
