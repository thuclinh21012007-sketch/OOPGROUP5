#pragma once
// =====================================================================
// [TV3] MultiCloudOptimizationService.h - Lop dieu phoi toan he thong
// Ke thua ICostService: ket noi TV1, TV2, TV3, TV4 phuc vu UI (TV5)
// =====================================================================
#include "../common/ICostService.h"
#include "CostCalculator.h"
#include "../tv4/OptimizationStrategy.h"
#include <memory>
#include <string>
#include <vector>

using namespace std;

class MultiCloudOptimizationService : public ICostService {
    CostCalculator calculator;
    vector<unique_ptr<ICostOptimizationStrategy>> strategies;
    string currentProvider;

public:
    MultiCloudOptimizationService(CostCalculator c,
                                  vector<unique_ptr<ICostOptimizationStrategy>> s,
                                  string cur);

    vector<ProviderCost> calculate(const vector<ResourceInput>& infra) override;
    vector<Recommendation> recommend(const vector<ResourceInput>& infra) override;

    CostCalculator& getCalculator() { return calculator; }
    const CostCalculator& getCalculator() const { return calculator; }
    const string& getCurrentProvider() const { return currentProvider; }
};
