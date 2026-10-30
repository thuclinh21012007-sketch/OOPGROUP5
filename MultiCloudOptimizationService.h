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

class MultiCloudOptimizationService : public ICostService {
    CostCalculator calculator;
    std::vector<std::unique_ptr<ICostOptimizationStrategy>> strategies;
    std::string currentProvider;

public:
    MultiCloudOptimizationService(CostCalculator c,
                                  std::vector<std::unique_ptr<ICostOptimizationStrategy>> s,
                                  std::string cur);

    std::vector<ProviderCost> calculate(const std::vector<ResourceInput>& infra) override;
    std::vector<Recommendation> recommend(const std::vector<ResourceInput>& infra) override;

    CostCalculator& getCalculator() { return calculator; }
    const CostCalculator& getCalculator() const { return calculator; }
    const std::string& getCurrentProvider() const { return currentProvider; }
};
