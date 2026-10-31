#pragma once
#include "OptimizationStrategy.h"

using namespace std;

// Chien luoc 3: Chuyen sang Reserved Instances / Savings Plans (giam 30%)
class PricingModelStrategy : public ICostOptimizationStrategy {
public:
    string name() const override;
    vector<Recommendation> optimize(const OptimizationContext& ctx) override;
};
