#pragma once
#include "OptimizationStrategy.h"

using namespace std;

// Chien luoc 1: Chuyen sang nha cung cap re nhat cho cung cau hinh
class CheapestProviderStrategy : public ICostOptimizationStrategy {
public:
    string name() const override;
    vector<Recommendation> optimize(const OptimizationContext& ctx) override;
};
