#include "PricingModelStrategy.h"
#include "../tv3/CostCalculator.h"

using namespace std;

string PricingModelStrategy::name() const {
    return "PricingModel";
}

vector<Recommendation> PricingModelStrategy::optimize(const OptimizationContext& ctx) {
    vector<Recommendation> recs;
    const double RESERVED_DISCOUNT = 0.30; // Gia dinh giam 30% khi dung cam ket

    ProviderCost pc = ctx.calculator.costFor(ctx.currentProvider, ctx.infra);
    if (pc.compute <= 0.0) return recs;

    double before = pc.total();
    double after  = before - pc.compute * RESERVED_DISCOUNT;

    Recommendation r;
    r.strategy = name();
    r.description = "Chay dai han: chuyen may chu sang Reserved/Savings Plans (gia dinh giam 30%)";
    r.before = before;
    r.after = after;
    recs.push_back(r);

    return recs;
}
