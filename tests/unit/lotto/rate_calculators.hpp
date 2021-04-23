#ifndef RATE_CALCULATORS_H
#define RATE_CALCULATORS_H

/*
 * Rate calculator that returns the same rate for every event id
 */
template <typename EventIDType>
class UniformRateCalculator
{
public:
    UniformRateCalculator(double rate) : rate(rate) {}
    double calculate_rate(const EventIDType& event_id) const { return rate; }
    double get_rate() const { return rate; }
    void set_rate(double new_rate) { rate = new_rate; }

private:
    double rate;
};

/*
 * Rate calulator that returns a rate of 1 for a single "hot" event id
 * and returns a rate of 0 for all other event ids
 */
template <typename EventIDType>
class OneHotRateCalculator
{
public:
    OneHotRateCalculator(const EventIDType& hot_id) : hot_id(hot_id) {}
    double calculate_rate(const EventIDType& event_id) const
    {
        if (event_id == hot_id)
        {
            return 1.0;
        }
        else
        {
            return 0.0;
        }
    }
    EventIDType get_hot_id() const { return hot_id; }
    void set_hot_id(const EventIDType& new_hot_id) { hot_id = new_hot_id; }

private:
    EventIDType hot_id;
};

/*
 *  Rate calculator that returns one rate for even number ids and another for odd number ids
 */
class EvenOddRateCalculator
{
public:
    EvenOddRateCalculator(double even_rate, double odd_rate) : even_rate(even_rate), odd_rate(odd_rate) {}
    double calculate_rate(const int& event_id) const
    {
        if (event_id % 2 == 0)
        {
            return even_rate;
        }
        else
        {
            return odd_rate;
        }
    }
    double get_even_rate() const { return even_rate; }
    double get_odd_rate() const { return odd_rate; }
    void set_even_rate(double new_rate) { even_rate = new_rate; }
    void set_odd_rate(double new_rate) { odd_rate = new_rate; }

private:
    double even_rate;
    double odd_rate;
};

#endif
