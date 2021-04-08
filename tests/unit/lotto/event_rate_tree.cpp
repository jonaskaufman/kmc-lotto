#include "lotto/random.hpp"
#include <gtest/gtest.h>
#include <lotto/event_rate_tree.hpp>
#include <lotto/event_rate_tree_impl.hpp>
#include <memory>

using ID = int;

class EventRateNodeDataTest : public testing::Test
{
protected:
    void SetUp() override
    {
        data_a_ptr = std::make_unique<lotto::EventRateNodeData<ID>>(id_a, rate_a);
        data_b_ptr = std::make_unique<lotto::EventRateNodeData<ID>>(id_b, rate_b);
    }

    // Event IDs
    ID id_a = 7;
    ID id_b = 5;

    // Initial rates
    double rate_a = 0.111;
    double rate_b = 0.666;

    // Pointers to node data
    std::unique_ptr<lotto::EventRateNodeData<ID>> data_a_ptr, data_b_ptr;

    // Default constructed node data (no event ID, zero rate)
    lotto::EventRateNodeData<ID> non_leaf_node_data;

    // Check if node data contains an event ID
    bool has_event_id(const lotto::EventRateNodeData<ID>& data) const { return data.event_id.has_value(); }
};

TEST_F(EventRateNodeDataTest, ConstructAndAccess)
{
    // Checks construction and access to rate, event ID
    EXPECT_EQ(data_a_ptr->get_rate(), rate_a);
    EXPECT_EQ(data_a_ptr->get_event_id(), id_a);
    EXPECT_EQ(data_b_ptr->get_rate(), rate_b);
    EXPECT_EQ(data_b_ptr->get_event_id(), id_b);
}

TEST_F(EventRateNodeDataTest, UpdateRateLeaf)
{
    // Checks rate update for leaf data (should update)
    double new_rate_a = data_a_ptr->get_rate() + 1.0;
    data_a_ptr->update_rate(new_rate_a);
    EXPECT_EQ(data_a_ptr->get_rate(), new_rate_a);
    double new_rate_b = data_b_ptr->get_rate() + 1.0;
    data_b_ptr->update_rate(new_rate_b);
    EXPECT_EQ(data_b_ptr->get_rate(), new_rate_b);
}

TEST_F(EventRateNodeDataTest, UpdateRateNonLeaf)
{
    // Checks rate update for non-leaf data (should not update)
    double old_rate = non_leaf_node_data.get_rate();
    double new_rate = old_rate + 1.0;
    non_leaf_node_data.update_rate(new_rate);
    EXPECT_EQ(non_leaf_node_data.get_rate(), old_rate);
}

TEST_F(EventRateNodeDataTest, Addition)
{
    // Checks addition of data
    double rate_sum = rate_a + rate_b;
    auto data_a_plus_data_b = *data_a_ptr + *data_b_ptr;
    EXPECT_EQ(data_a_plus_data_b.get_rate(), rate_sum);
    EXPECT_FALSE(has_event_id(data_a_plus_data_b));
}

class EventRateTreeTest : public testing::Test
{
};

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
