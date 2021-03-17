#ifndef EVENT_RATE_TREE_H
#define EVENT_RATE_TREE_H

#include <optional>

namespace lotto
{
/*
 * Node class to hold event rate data within a binary sum tree
 * For leaves, should contain an event ID and its corresponding rate
 * For non-leaves, should only contain a (summed) rate
 */
template <typename EventIDType>
class EventRateNode
{
public:
    // Construct as a leaf given an event ID and rate
    EventRateNode(const EventIDType& event_id, double rate) : event_id(event_id), rate(rate){};

    // Return the node's event ID (should only be called on leaves)
    const EventIDType& get_event_id() const { return event_id.value(); }

    // Return the node's rate
    double get_rate() const { return rate; }

    // Update a node's rate, if it stores no event ID
    void update_rate(double new_rate)
    {
        if (event_id.has_value())
        {
            rate = new_rate;
        }
        return;
    }

    // Return a new node with no event ID and summed rates
    EventRateNode operator+(const EventRateNode& rhs_node) const
    {
        EventRateNode summed_node;
        summed_node.rate = this->rate + rhs_node.rate;
        return summed_node;
    }

private:
    // Construct as a non-leaf with no event ID and zero rate
    EventRateNode() : event_id{std::nullopt}, rate(0.0){};

    // Event ID, which may not exist
    const std::optional<EventIDType> event_id;

    // Event rate or summed rate
    double rate;
};

} // namespace lotto
#endif
