#include "RightSizingStrategy.h"
#include "../tv3/CostCalculator.h"
#include <algorithm>

using namespace std;

string RightSizingStrategy::name() const {
    return "RightSizing";
}

vector<Recommendation> RightSizingStrategy::optimize(const OptimizationContext& ctx) {
    vector<Recommendation> recs;
    vector<ResourceInput> smaller = ctx.infra;
    bool changed = false;

    for (ResourceInput& r : smaller) {
        if (r.type == "COMPUTE" && r.usagePercent < 40 && r.vcpu > 1) {
            r.vcpu = max(1, r.vcpu / 2);
            r.ramGB = max(1, r.ramGB / 2);
            changed = true;
        }
    }

    if (!changed) return recs;

    double before = ctx.calculator.totalFor(ctx.currentProvider, ctx.infra);
    double after  = ctx.calculator.totalFor(ctx.currentProvider, smaller);

    if (after < before) {
        Recommendation r;
        r.strategy = name();
        r.description = "Ha vCPU/RAM cua may chu co muc su dung thap (< 40%) xuong mot nua";
        r.before = before;
        r.after = after;
        recs.push_back(r);
    }
    return recs;
}
