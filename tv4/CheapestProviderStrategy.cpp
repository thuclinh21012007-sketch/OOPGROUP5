#include "CheapestProviderStrategy.h"
#include "../tv3/CostCalculator.h"
#include <algorithm>

std::string CheapestProviderStrategy::name() const {
    return "CheapestProvider";
}

std::vector<Recommendation> CheapestProviderStrategy::optimize(const OptimizationContext& ctx) {
    std::vector<Recommendation> recs;
    auto all = ctx.calculator.calculateAll(ctx.infra);
    if (all.empty()) return recs;

    double current = ctx.calculator.totalFor(ctx.currentProvider, ctx.infra);
    auto best = std::min_element(all.begin(), all.end(),
        [](const ProviderCost& a, const ProviderCost& b) { return a.total() < b.total(); });

    if (best->total() < current) {
        Recommendation r;
        r.strategy = name();
        r.description = "Chuyen toan bo tu " + ctx.currentProvider + " sang " + best->provider;
        r.before = current;
        r.after = best->total();
        recs.push_back(r);
    }
    return recs;
}
