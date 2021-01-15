#ifndef REJECTION_FREE_H
#define REJECTION_FREE_H

#include "event_selector.hpp"
#include <map>
#include <vector>

namespace lotto
{

template <typename EventIDType, typename RateCalculatorType>
class RejectionFreeEventSelector : public EventSelectorBase<EventIDType, RateCalculatorType>
{
public:
    RejectionFreeEventSelector(RateCalculatorType* rate_calculator,
                               const std::vector<EventIDType>& event_id_list,
                               const std::map<EventIDType, std::vector<EventIDType>>& impact_table)
        : EventSelectorBase<EventIDType, RateCalculatorType>(rate_calculator), impact_table(impact_table)
    {
    }

private:
    const std::map<EventIDType, std::vector<EventIDType>> impact_table;
};
} // namespace lotto

#endif
