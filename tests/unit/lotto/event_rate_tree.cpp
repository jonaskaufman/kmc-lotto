#include "lotto/random.hpp"
#include "sequences.hpp"
#include "test_parameters.hpp"
#include <gtest/gtest.h>
#include <lotto/event_rate_tree.hpp>
#include <lotto/event_rate_tree_impl.hpp>
#include <memory>
#include <numeric>

class EventRateNodeDataTest : public testing::Test
{
protected:
    using ID = int;

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

    // Checks if node data contains an event ID
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
protected:
    using ID = int;

    void SetUp() override
    {
        // Reseed generator for testing
        generator.reseed_generator(TEST_SEED);

        // Set up event IDs
        init_ids = hashed_sequence(n_events);

        // Set up initial rates
        for (int i = 0; i < n_events; ++i)
        {
            init_rates.push_back(generator.sample_unit_interval());
        }

        // Set up tree
        tree_ptr = std::make_unique<lotto::EventRateTree<ID>>(init_ids, init_rates);
    }

    // Random generator
    lotto::RandomGenerator generator;

    // Pointer to event rate tree
    std::unique_ptr<lotto::EventRateTree<ID>> tree_ptr;

    // Number of events, initals IDs and rates
    int n_events = 1000;
    std::vector<ID> init_ids;
    std::vector<double> init_rates;

    // Gets map from event ID to index in the tree leaves
    const std::map<ID, lotto::Index>& event_to_leaf_index() const { return tree_ptr->event_to_leaf_index; }

    // Returns the event IDs of the tree leaves
    std::vector<ID> get_leaf_ids() const { return tree_ptr->leaf_ids(); }

    // Returns the rates of the tree leaves
    std::vector<double> get_leaf_rates() const { return tree_ptr->leaf_rates(); }

    // Returns the cumulative rates of the tree leaves
    std::vector<double> get_cumulative_leaf_rates() const
    {
        std::vector<double> cumulative_rates;
        double rate_sum = 0.0;
        for (double rate : get_leaf_rates())
        {
            rate_sum += rate;
            cumulative_rates.push_back(rate_sum);
        }
        return cumulative_rates;
    }
};

TEST_F(EventRateTreeTest, Construct)
{
    // Checks construction and data correctness
    auto leaf_ids = get_leaf_ids();
    auto leaf_rates = get_leaf_rates();
    auto rate_it = init_rates.begin();
    for (auto id_it = init_ids.begin(); id_it != init_ids.end(); ++id_it)
    {
        auto leaf_ix = event_to_leaf_index().at(*id_it);
        EXPECT_EQ(*id_it, leaf_ids[leaf_ix]);
        EXPECT_EQ(*rate_it, leaf_rates[leaf_ix]);
        ++rate_it;
    }
}

TEST_F(EventRateTreeTest, TotalRate)
{
    // Checks that the total rate returned is correct
    double rate_sum = std::accumulate(init_rates.begin(), init_rates.end(), 0.0);
    EXPECT_DOUBLE_EQ(tree_ptr->total_rate(), rate_sum);
}

TEST_F(EventRateTreeTest, UpdateRate)
{
    // Checks that the individual and total rates change appropriately upon updating
    int n_updates = 100;
    for (int i = 0; i < n_updates; ++i)
    {
        ID id_to_update = init_ids[generator.sample_integer_range(n_events - 1)];
        auto leaf_ix = event_to_leaf_index().at(id_to_update);
        double new_rate = generator.sample_unit_interval();
        double delta_rate = new_rate - get_leaf_rates()[leaf_ix];
        double old_total_rate = tree_ptr->total_rate();

        tree_ptr->update_rate(id_to_update, new_rate);
        EXPECT_EQ(new_rate, get_leaf_rates()[leaf_ix]);
        double new_total_rate = tree_ptr->total_rate();
        EXPECT_DOUBLE_EQ(new_total_rate, old_total_rate + delta_rate);
    }
}

TEST_F(EventRateTreeTest, RandomQuery)
{
    // Check thats querying the tree returns the correct event ID, based on the cumulative rates
    double total_rate = tree_ptr->total_rate();
    auto cumulative_rates = get_cumulative_leaf_rates();
    int n_queries = 100;
    for (int i = 0; i < n_queries; ++i)
    {
        double query_value = total_rate * generator.sample_unit_interval();
        ID result = tree_ptr->query_tree(query_value);
        auto result_leaf_ix = event_to_leaf_index().at(result);
        EXPECT_LE(query_value, cumulative_rates[result_leaf_ix]);
        if (result_leaf_ix != 0)
        {
            EXPECT_GT(query_value, cumulative_rates[result_leaf_ix - 1]);
        }
    }
}

TEST_F(EventRateTreeTest, EdgeQuery)
{
    // Checks that correct event is selected in edge case where query value is exactly equal to a cumulative rate

    // Set all rates to 1
    for (const ID& id : init_ids)
    {
        tree_ptr->update_rate(id, 1.0);
    }

    // Leaves indexed from 0, so leaf i should have cumulative rate i + 1
    // Query integers and check that the leaf with the correct index is chosen
    for (int i = 0; i < n_events; ++i)
    {
        int query_value = i + 1;
        ID result = tree_ptr->query_tree(query_value);
        auto result_leaf_ix = event_to_leaf_index().at(result);
        EXPECT_EQ(i, result_leaf_ix);
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
