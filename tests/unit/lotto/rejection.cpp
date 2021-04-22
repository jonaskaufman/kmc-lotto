#include "rate_calculators.hpp"
#include "sequences.hpp"
#include "statistics.hpp"
#include "test_parameters.hpp"
#include <gtest/gtest.h>
#include <lotto/rejection.hpp>
#include <memory>

class RejectionEventSelectorTest : public testing::Test
{
protected:
    using ID = int;

    void SetUp() override
    {
        // Set up event ID list
        event_id_list = hashed_sequence(n_events);

        // Set up rate calculators
        one_hot_calculator_ptr = std::make_shared<OneHotRateCalculator<ID>>(event_id_list[0]);
        double rate_upper_bound = 1.0;
        uniform_calculator_ptr = std::make_shared<UniformRateCalculator<ID>>(rate_upper_bound);

        // Set up event selectors
        one_hot_selector_ptr = std::make_unique<lotto::RejectionEventSelector<ID, OneHotRateCalculator<ID>>>(
            one_hot_calculator_ptr, rate_upper_bound, event_id_list);
        uniform_selector_ptr = std::make_unique<lotto::RejectionEventSelector<ID, UniformRateCalculator<ID>>>(
            uniform_calculator_ptr, rate_upper_bound, event_id_list);

        // Reseed selector generators for testing
        uniform_selector_ptr->reseed_generator(TEST_SEED);
        one_hot_selector_ptr->reseed_generator(TEST_SEED);
    }

    // Event ID list
    int n_events = 1000;
    std::vector<ID> event_id_list;

    // Rate calculator pointers
    std::shared_ptr<OneHotRateCalculator<ID>> one_hot_calculator_ptr;
    std::shared_ptr<UniformRateCalculator<ID>> uniform_calculator_ptr;

    // Event selectors, stored with pointers because they have no default constructor
    std::unique_ptr<lotto::RejectionEventSelector<ID, OneHotRateCalculator<ID>>> one_hot_selector_ptr;
    std::unique_ptr<lotto::RejectionEventSelector<ID, UniformRateCalculator<ID>>> uniform_selector_ptr;

    // Resets uniform event selector upper bound (and reseeds the generator)
    void reset_uniform_selector_rate_upper_bound(double new_rate_upper_bound)
    {
        uniform_selector_ptr = std::make_unique<lotto::RejectionEventSelector<ID, UniformRateCalculator<ID>>>(
            uniform_calculator_ptr, new_rate_upper_bound, event_id_list);
        uniform_selector_ptr->reseed_generator(TEST_SEED);
        return;
    }
};

TEST_F(RejectionEventSelectorTest, Construct)
{
    // Checks if RejectionEventSelector can be constructed
}

TEST_F(RejectionEventSelectorTest, CorrectEventSelection)
{
    // Checks if the correct event is selected when only one event is allowed
    for (const ID& expected_event_id : event_id_list)
    {
        one_hot_calculator_ptr->set_hot_id(expected_event_id);
        auto event_and_time = one_hot_selector_ptr->select_event();
        ID selected_event_id = event_and_time.first;
        EXPECT_EQ(selected_event_id, expected_event_id);
    }
}

TEST_F(RejectionEventSelectorTest, AverageTimeStep)
{
    // Checks if the average time step is correct when all events have rates equal to the rate upper bound

    // Loop over different rate upper bounds r0
    int n_rates = 100;
    double rate_step = 0.5;
    int n_samples = 1000000;
    for (int i = 1; i <= n_rates; ++i)
    {
        double r0 = i * rate_step;
        uniform_calculator_ptr->set_rate(r0);
        reset_uniform_selector_rate_upper_bound(r0);

        // Sample time steps
        std::vector<double> time_step_samples(n_samples);
        for (int j = 0; j < n_samples; ++j)
        {
            auto event_and_time = uniform_selector_ptr->select_event();
            time_step_samples[j] = event_and_time.second;
        }

        // Check average of samples
        check_samples_from_log_inverse_distribution(1.0 / (event_id_list.size() * r0), time_step_samples);
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
