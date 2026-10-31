#include "MultiCloudOptimizationService.h"
#include <algorithm>

using namespace std;

MultiCloudOptimizationService::MultiCloudOptimizationService(
    CostCalculator c,
    vector<unique_ptr<ICostOptimizationStrategy>> s,
    string cur)
    : calculator(move(c)), strategies(move(s)), currentProvider(move(cur)) {}

vector<ProviderCost> MultiCloudOptimizationService::calculate(const vector<ResourceInput>& infra) {
    return calculator.calculateAll(infra);
}

vector<Recommendation> MultiCloudOptimizationService::recommend(const vector<ResourceInput>& infra) {
    OptimizationContext ctx{infra, calculator, currentProvider};
    vector<Recommendation> all;
    for (auto& st : strategies) {
        auto recs = st->optimize(ctx);
        all.insert(all.end(), recs.begin(), recs.end());
    }
    // Sap xep khuyen nghi tiet kiem nhieu nhat len dau
    sort(all.begin(), all.end(),
        [](const Recommendation& a, const Recommendation& b) { return a.saving() > b.saving(); });
    return all;
}
