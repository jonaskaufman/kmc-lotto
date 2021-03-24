#ifndef SUM_TREE_HH
#define SUM_TREE_HH

#include <memory>
//#include <set>
#include <vector>

namespace lotto
{

/**
 * This class defines the nodes for the InvertedSumTree.
 * The nodes are "uspide down", with shared pointers going
 * to the parent nodes (rather than unique pointers going
 * to the child nodes).
 */

template <typename NodeType>
struct InvertedBinaryTreeNode
{

public:
    typedef InvertedBinaryTreeNode Node;

    /// Default constructor sets parent to null value, but must know who its children are. Requires default constructor
    /// for data type

    InvertedBinaryTreeNode(Node* left_node_ptr, Node* right_node_ptr, const NodeType& init_data);

    /// Holds an arbitrary type of object that we're interested in
    NodeType data;

    /// Points to the left child node. If the value is null then *this is a leaf in the tree
    Node* left_child;

    /// Points to the right child node. If the value is null then *this is a leaf in the tree
    Node* right_child;

    /// Points to the parent node. If the value is null then *this is the root of the tree
    std::shared_ptr<Node> parent;

private:
};

/**
 * The InvertedBinarySumTree is a binary tree that is constructed
 * "upside down". The constructor takes a range of values that
 * define the outermost values (leaves). Nodes are added accordingly,
 * pairing up leaves together into parent nodes. These nodes
 * in turn are also paired up, until they converge to a single node.
 */

template <typename NodeType>
class InvertedBinarySumTree
{

public:
    typedef InvertedBinaryTreeNode<NodeType> Node;
    typedef long int size_type;

    /// Initialize the tree with a range of values to have at the leaves
    template <typename NodeTypeIterType>
    InvertedBinarySumTree(const NodeTypeIterType& init_begin, const NodeTypeIterType& init_end);

    /// Initialize the tree with a vector of values
    InvertedBinarySumTree(const std::vector<NodeType>& init_leaf_values);

    /// Initialize with an empty tree, insert values after construction
    InvertedBinarySumTree<NodeType>();

    /// Print the tree to cout for visualization
    void print() const;

    /// Return pointer to the root of the tree
    const Node* root() const;

    /// Return reference to the pointers of the leaves of the tree
    const std::vector<std::unique_ptr<Node>>& leaves() const;

    /// Change values of a leaf and resum the tree
    void update(int leaf_idx, const NodeType& val);

    /// Change updates values of a leaf and resum the tree
    // void update_internals(int leaf_idx);
private:
    /// Points to the leaves of the tree
    const std::vector<std::unique_ptr<Node>> m_leaves;

    /// Points to the root of the tree
    const Node* m_root;

    /// Recursively print node values to cout
    void _postorder_print(const Node* const p, int indent = 0) const;

    /// Used for construction indirectly. Create a leaf for each of the given data values and save them.
    template <typename NodeTypeIterType>
    std::vector<std::unique_ptr<Node>> _populated_leaves(const NodeTypeIterType& init_begin,
                                                         const NodeTypeIterType& init_end);

    /// Used for construction. Join up leaves pair by pair into parent nodes, and then join these parents
    /// together in pairs, until only one parent exists (the root)
    void _multilevel_join(std::vector<std::unique_ptr<Node>>& init_leaves);

    /// Used for construction. Create leaves for each data value provided via _populated_leaves() and then extend tree
    /// through _multilevel_join()
    template <typename NodeTypeIterType>
    std::vector<std::unique_ptr<Node>> _populated_and_joined_leaves(const NodeTypeIterType& init_begin,
                                                                    const NodeTypeIterType& init_end);

    /// Pair up the given vector of nodes into parent nodes, and return a vector of the parent nodes
    std::vector<Node*> _multi_join(std::vector<Node*>& current_parent_nodes);

    /// Given two child nodes, create a new parent node they are both joined to, and return the parent node
    Node* _join(Node* left_child, Node* right_child);

    /// Given two node pointers, sum the values of each node together, keeping into account one may be null
    NodeType _summed_node_data(const Node* const& left_child, const Node* const& right_child);

    /// Using a given node as a starting point, walk through the parents until you find the root
    Node* _find_root(Node* starting_node) const;

    /// Given a leaf node index that has changed resum its parents until the root node
    void _resum_to_top(int leaf_idx);

    /// Given a leaf node index that has changed resum its parents until the root node
    Node* _resum_parent(Node* node);
};
} // namespace lotto

#endif
