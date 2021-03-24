#ifndef EVENT_RATE_TREE_H
#define EVENT_RATE_TREE_H

#include "sum_tree.hpp"
#include <map>
#include <optional>

namespace lotto
{
/*
 * Node class to hold event rate data within a binary sum tree
 * For leaves, should contain an event ID and its corresponding rate
 * For non-leaves, should only contain a (summed) rate
 */
template <typename EventIDType>
class EventRateNode // rename to node data or something?
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

/*
 * Tree of event rates, with events as leaves
 */
template <typename EventIDType>
class EventRateTree
{
public:
    // Construct tree given list of event IDs and corresponding initial rates
    EventRateTree(const std::vector<EventIDType>& all_event_ids, const std::vector<double>& all_rates);

    // Traverse tree and return the event ID of event at index i
    // for which R(i-1) < u <= R(i), where u is the query value
    // and R(i) is cumulative rate of all events up to and including event i
    const EventIDType& query_tree(double query_value) const;
    // Should this take scale for total rate automatically?

    // Update the rate of a specific event
    void update_rate(const EventIDType& event_id, double new_rate);

    // Return the total rate of all events stored in tree
    double total_rate() const;

private:
    using NodeData = EventRateNode<EventIDType>;
    using Node = InvertedBinaryTreeNode<NodeData>;
    using Index = int;

    //
    InvertedBinarySumTree<NodeData> event_rate_tree;

    //
    const std::map<EventIDType, Index> event_to_leaf_index; // needed for updating tree itself given an EventID

    //
    std::vector<NodeData> events_as_leaves(const std::vector<EventIDType>& init_events,
                                           const std::vector<double>& init_rates);

    //
    std::map<EventIDType, Index> event_to_leaf_index_map() const;

    //
    const Node* bifurcate(const Node* current_node_ptr, double& running_rate) const;
};

} // namespace lotto
#endif
