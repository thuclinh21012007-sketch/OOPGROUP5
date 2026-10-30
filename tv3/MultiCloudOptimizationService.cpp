#include "MultiCloudOptimizationService.h"
#include <algorithm>

MultiCloudOptimizationService::MultiCloudOptimizationService(
    CostCalculator c,
    std::vector<std::unique_ptr<ICostOptimizationStrategy>> s,
    std::string cur)
    : calculator(std::move(c)), strategies(std::move(s)), currentProvider(std::move(cur)) {}

std::vector<ProviderCost> MultiCloudOptimizationService::calculate(const std::vector<ResourceInput>& infra) {
    return calculator.calculateAll(infra);
}

std::vector<Recommendation> MultiCloudOptimizationService::recommend(const std::vector<ResourceInput>& infra) {
    OptimizationContext ctx{infra, calculator, currentProvider};
    std::vector<Recommendation> all;
    for (auto& st : strategies) {
        auto recs = st->optimize(ctx);
        all.insert(all.end(), recs.begin(), recs.end());
    }
    // Sap xep khuyen nghi tiet kiem nhieu nhat len dau
    std::sort(all.begin(), all.end(),
        [](const Recommendation& a, const Recommendation& b) { return a.saving() > b.saving(); });
    return all;
}
