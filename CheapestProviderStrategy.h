#pragma once
#include "OptimizationStrategy.h"

// Chien luoc 1: Chuyen sang nha cung cap re nhat cho cung cau hinh
class CheapestProviderStrategy : public ICostOptimizationStrategy {
public:
    std::string name() const override;
    std::vector<Recommendation> optimize(const OptimizationContext& ctx) override;
};
