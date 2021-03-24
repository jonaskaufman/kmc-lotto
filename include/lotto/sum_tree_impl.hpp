#ifndef SUM_TREE_IMPL_HH
#define SUM_TREE_IMPL_HH

#include "sum_tree.hpp"
#include <cassert>
#include <iomanip>
#include <iostream>
#include <iterator>

namespace lotto
{

//*******************************************************************************************************//

template <typename NodeType>
InvertedBinaryTreeNode<NodeType>::InvertedBinaryTreeNode(Node* left_node_ptr,
                                                         Node* right_node_ptr,
                                                         const NodeType& init_data)
    : parent(nullptr), left_child(left_node_ptr), right_child(right_node_ptr), data(init_data)
{
}

//*******************************************************************************************************//

template <typename NodeType>
template <typename NodeTypeIterType>
InvertedBinarySumTree<NodeType>::InvertedBinarySumTree(const NodeTypeIterType& init_begin,
                                                       const NodeTypeIterType& init_end)
    : m_leaves(std::move(this->_populated_and_joined_leaves(init_begin, init_end))),
      m_root(this->_find_root(m_leaves[0].get()))
{
}

template <typename NodeType>
InvertedBinarySumTree<NodeType>::InvertedBinarySumTree(const std::vector<NodeType>& init_leaf_values)
    : InvertedBinarySumTree(init_leaf_values.begin(), init_leaf_values.end())
{
}

template <typename NodeType>
typename InvertedBinarySumTree<NodeType>::Node* InvertedBinarySumTree<NodeType>::_find_root(Node* starting_node) const
{
    Node* curr_parent = starting_node->parent.get();
    Node* root_node = starting_node;

    while (curr_parent != nullptr)
    {
        root_node = curr_parent;
        curr_parent = root_node->parent.get();
    }

    return root_node;
}

template <typename NodeType>
void InvertedBinarySumTree<NodeType>::print() const
{
    this->_postorder_print(m_root);
    return;
}

template <typename NodeType>
const typename InvertedBinarySumTree<NodeType>::Node* InvertedBinarySumTree<NodeType>::root() const
{
    return m_root;
}

template <typename NodeType>
const std::vector<std::unique_ptr<typename InvertedBinarySumTree<NodeType>::Node>>&
InvertedBinarySumTree<NodeType>::leaves() const
{
    return m_leaves;
}

template <typename NodeType>
void InvertedBinarySumTree<NodeType>::update(int leaf_idx, const NodeType& val)
{
    m_leaves[leaf_idx]->data = val;
    _resum_to_top(leaf_idx);
    return;
}

/*  template<typename NodeType>
  void InvertedBinarySumTree<NodeType>::update_internals(int leaf_idx){
                        m_leaves[leaf_idx]->data.update();
                        _resum_to_top(leaf_idx);
                return;
  }
*/

template <typename NodeType>
void InvertedBinarySumTree<NodeType>::_resum_to_top(int leaf_idx)
{
    Node* node_ptr = m_leaves[leaf_idx].get();
    while (node_ptr != root())
    {
        node_ptr = _resum_parent(node_ptr);
    }
    return;
}

template <typename NodeType>
typename InvertedBinarySumTree<NodeType>::Node* InvertedBinarySumTree<NodeType>::_resum_parent(Node* node)
{
    auto parent_ptr = node->parent.get();
    parent_ptr->data = _summed_node_data(parent_ptr->left_child, parent_ptr->right_child);
    return parent_ptr;
}

template <typename NodeType>
void InvertedBinarySumTree<NodeType>::_postorder_print(const Node* const p, int indent) const
{
    if (p != nullptr)
    {
        if (p->left_child)
        {
            this->_postorder_print(p->left_child, indent + 4);
        }
        if (p->right_child)
        {
            this->_postorder_print(p->right_child, indent + 4);
        }
        if (indent)
        {
            std::cout << std::setw(indent) << ' ';
        }
        std::cout << p->data << std::endl;
    }

    return;
}

template <typename NodeType>
template <typename NodeTypeIterType>
std::vector<std::unique_ptr<typename InvertedBinarySumTree<NodeType>::Node>>
InvertedBinarySumTree<NodeType>::_populated_leaves(const NodeTypeIterType& init_begin, const NodeTypeIterType& init_end)
{
    std::vector<std::unique_ptr<Node>> init_leaves;

    // Make room for all the leaves
    init_leaves.reserve(std::distance(init_begin, init_end));

    // Make a leaf for each value of data
    for (auto it = init_begin; it != init_end; ++it)
    {
        // The leaves have no children
        std::unique_ptr<Node> leaf_ptr(new Node(nullptr, nullptr, *it));
        init_leaves.push_back(std::move(leaf_ptr));
    }

    return init_leaves;
}

template <typename NodeType>
template <typename NodeTypeIterType>
std::vector<std::unique_ptr<typename InvertedBinarySumTree<NodeType>::Node>>
InvertedBinarySumTree<NodeType>::_populated_and_joined_leaves(const NodeTypeIterType& init_begin,
                                                              const NodeTypeIterType& init_end)
{
    /// Get all those leaves stored
    std::vector<std::unique_ptr<Node>> init_leaves(this->_populated_leaves(init_begin, init_end));

    // You got the leaves, now pair them up into nodes
    this->_multilevel_join(init_leaves);

    return init_leaves;
}

template <typename NodeType>
NodeType InvertedBinarySumTree<NodeType>::_summed_node_data(const Node* const& left_child,
                                                            const Node* const& right_child)
{
    // If both children are null, you should not be calling this
    assert(left_child != nullptr || right_child != nullptr);

    if (left_child == nullptr)
    {
        return right_child->data;
    }

    else if (right_child == nullptr)
    {
        return left_child->data;
    }

    else
    {
        return left_child->data + right_child->data;
    }
}

template <typename NodeType>
typename InvertedBinarySumTree<NodeType>::Node* InvertedBinarySumTree<NodeType>::_join(Node* left_child,
                                                                                       Node* right_child)
{
    // The parent node will point to the two children, and it's data value will be the sum of the
    // data stored in each of the children
    std::shared_ptr<Node> parent_ptr(
        std::make_shared<Node>(left_child, right_child, this->_summed_node_data(left_child, right_child)));

    // both children have the same parent, set them if possible
    if (left_child != nullptr)
    {
        left_child->parent = parent_ptr;
    }
    if (right_child != nullptr)
    {
        right_child->parent = parent_ptr;
    }

    return parent_ptr.get();
}

template <typename NodeType>
std::vector<typename InvertedBinarySumTree<NodeType>::Node*>
InvertedBinarySumTree<NodeType>::_multi_join(std::vector<Node*>& current_parent_nodes)
{
    std::vector<Node*> new_parent_nodes;

    // Pad the current set of parent nodes with a null pointer, so that there's an even number of them
    if (current_parent_nodes.size() % 2 != 0 && current_parent_nodes.size() != 1)
    {
        current_parent_nodes.push_back(nullptr);
    }

    // Grab two elements at a time, and join them into a parent node
    for (int i = 0; i < current_parent_nodes.size(); ++i)
    {
        new_parent_nodes.push_back(this->_join(current_parent_nodes[i], current_parent_nodes[i + 1]));
        i++;
    }

    return new_parent_nodes;
}

template <typename NodeType>
void InvertedBinarySumTree<NodeType>::_multilevel_join(std::vector<std::unique_ptr<Node>>& init_leaves)
{
    // We start with only the outermost leaves, at this point, all the leaves have no parents
    std::vector<Node*> current_highest_ancestry_level;
    current_highest_ancestry_level.reserve(init_leaves.size());

    for (auto it = init_leaves.begin(); it != init_leaves.end(); ++it)
    {
        current_highest_ancestry_level.push_back(it->get());
    }

    // Join pairs of nodes, until you end up at one node
    while (current_highest_ancestry_level.size() > 1)
    {
        current_highest_ancestry_level = this->_multi_join(current_highest_ancestry_level);
    }

    // Hoorah! Your highest level has a single node now. The tree is complete.
    return;
}

//*******************************************************************************************************//
//*******************************************************************************************************//

} // namespace lotto

#endif
