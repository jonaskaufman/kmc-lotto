#ifndef EVENT_RATE_TREE_IMPL_H
#define EVENT_RATE_TREE_IMPL_H

#include "event_rate_tree.hpp"
#include "sum_tree.hpp"
#include "sum_tree_impl.hpp"

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

template <typename EventIDType>
using NodeData = EventRateNodeData<EventIDType>;

template <typename EventIDType>
using Node = InvertedBinaryTreeNode<NodeData<EventIDType>>;

template <typename EventIDType>
EventRateTree<EventIDType>::EventRateTree(const std::vector<EventIDType>& all_event_ids,
                                          const std::vector<double>& all_rates)
    : event_rate_tree(this->events_as_leaves(all_event_ids, all_rates)),
      event_to_leaf_index(this->event_to_leaf_index_map())
{
}

template <typename EventIDType>
const EventIDType& EventRateTree<EventIDType>::query_tree(double query_value) const
{
    // TODO: Check that query value does not exceed total rate
    auto next_node_ptr = event_rate_tree.root();
    const Node* last_node_ptr = nullptr;
    while (next_node_ptr != nullptr)
    {
        last_node_ptr = next_node_ptr;
        next_node_ptr = this->bifurcate(next_node_ptr, query_value);
    }
    return last_node_ptr->data.get_event_id();
}

template <typename EventIDType>
void EventRateTree<EventIDType>::update_rate(const EventIDType& event_id, double new_rate)
{
    auto leaf_ix = event_to_leaf_index.at(event_id);
    NodeData& event_data = event_rate_tree.leaves()[leaf_ix]->data;
    event_data.update_rate(new_rate);
    event_rate_tree.update(leaf_ix, event_data);
}

template <typename EventIDType>
double EventRateTree<EventIDType>::total_rate() const
{
    return event_rate_tree.root()->data.get_rate();
}

template <typename EventIDType>
std::map<EventIDType, Index> EventRateTree<EventIDType>::event_to_leaf_index_map() const
{
    std::map<EventIDType, Index> index_map;
    const auto& tree_leaves = this->event_rate_tree.leaves();
    for (Index leaf_ix = 0; leaf_ix < tree_leaves.size(); ++leaf_ix)
    {
        const EventIDType& leaf_event_id = tree_leaves[leaf_ix].get()->data.get_event_id();
        index_map[leaf_event_id] = leaf_ix;
    }
    return index_map;
}

template <typename EventIDType>
std::vector<EventRateNodeData<EventIDType>>
EventRateTree<EventIDType>::events_as_leaves(const std::vector<EventIDType>& init_events,
                                             const std::vector<double>& init_rates) const
{
    std::vector<EventRateNodeData<EventIDType>> event_leaves;
    auto rate_it = init_rates.begin();
    for (auto event_it = init_events.begin(); event_it != init_events.end(); ++event_it)
    {
        event_leaves.emplace_back(*event_it, *rate_it);
        ++rate_it;
    }
    return event_leaves;
}

template <typename EventIDType>
const Node<EventIDType>* EventRateTree<EventIDType>::bifurcate(const Node* current_node_ptr, double& running_rate) const
{
    if (current_node_ptr->left_child == nullptr && current_node_ptr->right_child == nullptr)
    {
        return nullptr;
    }

    if (current_node_ptr->left_child != nullptr && running_rate < current_node_ptr->left_child->data.get_rate())
    {
        return current_node_ptr->left_child;
    }
    else
    {
        if (current_node_ptr->left_child != nullptr)
        {
            running_rate -= current_node_ptr->left_child->data.get_rate();
        }
        return current_node_ptr->right_child;
    }
}

template <typename EventIDType>
std::vector<NodeData<EventIDType>> EventRateTree<EventIDType>::leaf_data() const
{
    std::vector<NodeData> leaf_data;
    const auto& tree_leaves = this->event_rate_tree.leaves();
    for (auto leaf_it = tree_leaves.begin(); leaf_it != tree_leaves.end(); ++leaf_it)
    {
        leaf_data.emplace_back(leaf_it->get()->data);
    }
    return leaf_data;
}

template <typename EventIDType>
std::vector<EventIDType> EventRateTree<EventIDType>::leaf_ids() const
{
    auto leaf_data = this->leaf_data();
    std::vector<EventIDType> leaf_ids;
    for (auto data : leaf_data)
    {
        leaf_ids.push_back(data.get_event_id());
    }
    return leaf_ids;
}

template <typename EventIDType>
std::vector<double> EventRateTree<EventIDType>::leaf_rates() const
{
    auto leaf_data = this->leaf_data();
    std::vector<double> leaf_rates;
    for (auto data : leaf_data)
    {
        leaf_rates.push_back(data.get_rate());
    }
    return leaf_rates;
}

} // namespace lotto
#endif
