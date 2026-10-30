#pragma once
#include "OptimizationStrategy.h"

// Chien luoc 2: Ha cau hinh cac may chu su dung tai thap (< 40%)
class RightSizingStrategy : public ICostOptimizationStrategy {
public:
    std::string name() const override;
    std::vector<Recommendation> optimize(const OptimizationContext& ctx) override;
};
