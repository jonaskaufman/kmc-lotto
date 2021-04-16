#include "rate_calculators.hpp"
#include "statistics.hpp"
#include "test_parameters.hpp"
#include <gtest/gtest.h>
#include <lotto/rejection.hpp>
#include <memory>

using ID = int;

class RejectionEventSelectorTest : public testing::Test
{
protected:
    void SetUp() override
    {
        // Set up event id list
        int n_events = 1000;
        event_id_list = std::vector<ID>(n_events);
        for (int i = 0; i < n_events; ++i)
        {
            event_id_list[i] = i;
        }

        // Set up rate calculators
        double rate_upper_bound = 1.0;
        uniform_calculator_ptr = std::make_shared<UniformRateCalculator<ID>>(rate_upper_bound);
        one_hot_calculator_ptr = std::make_shared<OneHotRateCalculator<ID>>(event_id_list[0]);

        // Set up event selectors
        uniform_rates_selector_ptr = std::make_unique<lotto::RejectionEventSelector<ID, UniformRateCalculator<ID>>>(
            uniform_calculator_ptr, rate_upper_bound, event_id_list);
        uniform_rates_selector_ptr->reseed_generator(TEST_SEED); // fixed seed for testing
        one_hot_rates_selector_ptr = std::make_unique<lotto::RejectionEventSelector<ID, OneHotRateCalculator<ID>>>(
            one_hot_calculator_ptr, rate_upper_bound, event_id_list);
        one_hot_rates_selector_ptr->reseed_generator(TEST_SEED); // fixed seed for testing
    }

    // Event id list
    std::vector<ID> event_id_list;

    // Rate calculators
    std::shared_ptr<UniformRateCalculator<ID>> uniform_calculator_ptr;
    std::shared_ptr<OneHotRateCalculator<ID>> one_hot_calculator_ptr;

    // Event selectors, stored with pointers because they have no default constructor
    std::unique_ptr<lotto::RejectionEventSelector<ID, UniformRateCalculator<ID>>> uniform_rates_selector_ptr;
    std::unique_ptr<lotto::RejectionEventSelector<ID, OneHotRateCalculator<ID>>> one_hot_rates_selector_ptr;
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
        auto event_and_time = one_hot_rates_selector_ptr->select_event();
        ID selected_event_id = event_and_time.first;
        EXPECT_EQ(selected_event_id, expected_event_id);
    }
}

TEST_F(RejectionEventSelectorTest, AverageTimeStep)
{
    // Checks if the average time step is correct when all events have rates equal to the rate upper bound
    int n_steps = 100;
    double max_rate_upper_bound = 100.0;
    double rate_upper_bound_step = max_rate_upper_bound / n_steps;
    int n_samples = 1000000;

    // Loop over different rate upper bounds r0
    for (int i = 1; i <= n_steps; ++i)
    {
        double r0 = i * rate_upper_bound_step;
        uniform_calculator_ptr->set_rate(r0);
        uniform_rates_selector_ptr = std::make_unique<lotto::RejectionEventSelector<ID, UniformRateCalculator<ID>>>(
            uniform_calculator_ptr, r0, event_id_list);
        // TODO: Reseed generator for the new selector
    
        // Sample time steps
        std::vector<double> time_step_samples(n_samples);
        for (int j = 0; j < n_samples; ++j)
        {
            auto event_and_time = uniform_rates_selector_ptr->select_event();
            time_step_samples[j] = event_and_time.second;
        }

        // Note that the distribution ln(1/x) where  0 < x <= 1 has mean and standard deviation 1
        double expected_mean_time_step = 1.0 / (event_id_list.size() * r0);
        double sigma_of_time_step = expected_mean_time_step;
        double mean_time_step = get_mean(time_step_samples);
        double sigma_of_mean = standard_error_of_mean(sigma_of_time_step, time_step_samples.size());
        // Check that mean time step is within TEST_SIGMA standard deviations of expected value
        check_deviation_of_mean(mean_time_step, expected_mean_time_step, sigma_of_mean, TEST_SIGMA);
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
