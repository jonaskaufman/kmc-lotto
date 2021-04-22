#include "rate_calculators.hpp"
#include "sequences.hpp"
#include "statistics.hpp"
#include "test_parameters.hpp"
#include <gtest/gtest.h>
#include <lotto/rejection_free.hpp>
#include <memory>

class RejectionFreeEventSelectorTest : public testing::Test
{
protected:
    using ID = int;

    void SetUp() override
    {
        // Set up event ID list
        event_ids = hashed_sequence(n_events);

        // Set up impact tables
        std::map<ID, std::vector<ID>> complete_impact_table;
        std::map<ID, std::vector<ID>> neighbor_impact_table;
        for (int i = 0; i < n_events; ++i)
        {
            ID id = event_ids[i];
            complete_impact_table[id] = event_ids;
            neighbor_impact_table[id] = {id, event_ids[(i + 1) % n_events]};
        }

        // Set up rate calculators
        one_hot_calculator_ptr = std::make_shared<OneHotRateCalculator<ID>>(event_ids[0]);
        uniform_calculator_ptr = std::make_shared<UniformRateCalculator<ID>>(1.0);

        // Set up event selectors
        one_hot_selector_ptr = std::make_unique<lotto::RejectionFreeEventSelector<ID, OneHotRateCalculator<ID>>>(
            one_hot_calculator_ptr, event_ids, neighbor_impact_table);
        uniform_selector_ptr = std::make_unique<lotto::RejectionFreeEventSelector<ID, UniformRateCalculator<ID>>>(
            uniform_calculator_ptr, event_ids, complete_impact_table);

        // Reseed selector generators for testing
        one_hot_selector_ptr->reseed_generator(TEST_SEED);
        uniform_selector_ptr->reseed_generator(TEST_SEED);
    }

    // Event ID list
    int n_events = 1000;
    std::vector<ID> event_ids;

    // Rate calculator pointers
    std::shared_ptr<OneHotRateCalculator<ID>> one_hot_calculator_ptr;
    std::shared_ptr<UniformRateCalculator<ID>> uniform_calculator_ptr;

    // Event selectors, stored with pointers because they have no default constructor
    std::unique_ptr<lotto::RejectionFreeEventSelector<ID, OneHotRateCalculator<ID>>> one_hot_selector_ptr;
    std::unique_ptr<lotto::RejectionFreeEventSelector<ID, UniformRateCalculator<ID>>> uniform_selector_ptr;
};

TEST_F(RejectionFreeEventSelectorTest, Construct)
{
    // Checks if RejectionFreeEventSelector can be constructed
}

TEST_F(RejectionFreeEventSelectorTest, CorrectEventSelection)
{
    // Checks if the correct event is selected when only one event is allowed
    for (const ID& expected_event_id : event_ids)
    {
        one_hot_calculator_ptr->set_hot_id(expected_event_id);
        auto event_and_time = one_hot_selector_ptr->select_event();
        ID selected_event_id = event_and_time.first;
        EXPECT_EQ(selected_event_id, expected_event_id);
    }
}

TEST_F(RejectionFreeEventSelectorTest, AverageTimeStep)
{
    // Checks if the average time step is correct when all events have the same rate

    // Loop over different rates r0
    int n_rates = 10;
    double rate_step = 0.5;
    int n_samples = 10000;
    for (int i = 1; i <= n_rates; ++i)
    {
        double r0 = i * rate_step;
        uniform_calculator_ptr->set_rate(r0);

        // Sample time steps
        std::vector<double> time_step_samples(n_samples);
        for (int j = 0; j < n_samples; ++j)
        {
            auto event_and_time = uniform_selector_ptr->select_event();
            time_step_samples[j] = event_and_time.second;
        }
        check_samples_from_log_inverse_distribution(1.0 / (event_ids.size() * r0), time_step_samples);
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
