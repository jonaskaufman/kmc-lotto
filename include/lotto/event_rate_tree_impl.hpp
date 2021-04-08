#ifndef EVENT_RATE_TREE_IMPL_H
#define EVENT_RATE_TREE_IMPL_H

#include "event_rate_tree.hpp"

namespace lotto
{

template <typename EventIDType>
EventRateNodeData<EventIDType>::EventRateNodeData(const EventIDType& event_id, double rate)
    : event_id(event_id), rate(rate)
{
}

template <typename EventIDType>
const EventIDType& EventRateNodeData<EventIDType>::get_event_id() const
{
    return event_id.value();
}

template <typename EventIDType>
double EventRateNodeData<EventIDType>::get_rate() const
{
    return rate;
}

template <typename EventIDType>
void EventRateNodeData<EventIDType>::update_rate(double new_rate)
{
    if (event_id.has_value())
    {
        rate = new_rate;
    }
    return;
}

template <typename EventIDType>
EventRateNodeData<EventIDType>
EventRateNodeData<EventIDType>::operator+(const EventRateNodeData<EventIDType>& rhs_node) const
{
    EventRateNodeData<EventIDType> summed_node;
    summed_node.rate = this->rate + rhs_node.rate;
    return summed_node;
}

template <typename EventIDType>
EventRateNodeData<EventIDType>::EventRateNodeData() : event_id{std::nullopt}, rate(0.0){};

} // namespace lotto

#endif
