#ifndef EVENT_RATE_TREE_H
#define EVENT_RATE_TREE_H

#include "sum_tree.hpp"
#include <map>
#include <optional>

namespace lotto
{
/*
 * Class to hold event rate data within a binary sum tree node
 * For leaves, should contain an event ID and its corresponding rate
 * For non-leaves, should only contain a (summed) rate
 */
template <typename EventIDType>
class EventRateNodeData
{
public:
    // Construct as a leaf given an event ID and rate
    EventRateNodeData(const EventIDType& event_id, double rate);

    // Return the node's event ID (should only be called on leaves)
    const EventIDType& get_event_id() const;

    // Return the node's rate
    double get_rate() const;

    // Update a node's rate, if it stores no event ID
    void update_rate(double new_rate);

    // Return a new node with no event ID and summed rates
    EventRateNodeData operator+(const EventRateNodeData& rhs_node) const;

private:
    // Construct as a non-leaf with no event ID and zero rate
    EventRateNodeData();

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
    using NodeData = EventRateNodeData<EventIDType>;
    using Node = InvertedBinaryTreeNode<NodeData>;
    using Index = int;

    //
    InvertedBinarySumTree<NodeData> event_rate_tree;

    //
    const std::map<EventIDType, Index> event_to_leaf_index; // needed for updating tree itself given an EventID

    //
    std::vector<NodeData> events_as_leaves(const std::vector<EventIDType>& init_events,
                                           const std::vector<double>& init_rates) const;

    //
    std::map<EventIDType, Index> event_to_leaf_index_map() const;

    //
    const Node* bifurcate(const Node* current_node_ptr, double& running_rate) const;
};

} // namespace lotto
#endif
