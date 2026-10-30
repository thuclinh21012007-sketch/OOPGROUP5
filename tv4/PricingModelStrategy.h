#pragma once
#include "OptimizationStrategy.h"

// Chien luoc 3: Chuyen sang Reserved Instances / Savings Plans (giam 30%)
class PricingModelStrategy : public ICostOptimizationStrategy {
public:
    std::string name() const override;
    std::vector<Recommendation> optimize(const OptimizationContext& ctx) override;
};
