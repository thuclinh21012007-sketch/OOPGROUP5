#include "CostCalculator.h"
#include <algorithm>

using namespace std;

CostCalculator::CostCalculator(vector<ProviderBundle> p, Region r)
    : providers(move(p)), region(move(r)) {}

vector<string> CostCalculator::getProviderNames() const {
    vector<string> names;
    for (const auto& pb : providers) names.push_back(pb.name);
    return names;
}

ProviderCost CostCalculator::costFor(const string& providerName, const vector<ResourceInput>& infra) const {
    ProviderCost pc{providerName, 0, 0, 0};
    for (const ProviderBundle& pb : providers) {
        if (pb.name != providerName) continue;
        for (const ResourceInput& r : infra) {
            Specification s;
            if (r.type == "COMPUTE") {
                auto res = pb.factory->createCompute(r.vcpu, r.ramGB);
                s.vcpu = res->vcpu;
                s.ramGB = res->ramGB;
                pc.compute += r.quantity * pb.adapter->getPrice(ResourceType::Compute, s, region);
            } else if (r.type == "STORAGE") {
                auto res = pb.factory->createStorage(r.sizeGB);
                s.sizeGB = res->sizeGB;
                pc.storage += pb.adapter->getPrice(ResourceType::Storage, s, region);
            } else if (r.type == "DATABASE") {
                auto res = pb.factory->createDatabase(r.engine);
                s.engine = res->engine;
                pc.database += pb.adapter->getPrice(ResourceType::Database, s, region);
            }
        }
    }
    return pc;
}

double CostCalculator::totalFor(const string& providerName, const vector<ResourceInput>& infra) const {
    return costFor(providerName, infra).total();
}

vector<ProviderCost> CostCalculator::calculateAll(const vector<ResourceInput>& infra) const {
    vector<ProviderCost> out;
    for (const ProviderBundle& pb : providers) {
        out.push_back(costFor(pb.name, infra));
    }
    return out;
}

ProviderCost CostCalculator::costForByTime(const string& providerName,
                                           const vector<ResourceInput>& infra,
                                           TimeUnit unit) const {
    ProviderCost monthly = costFor(providerName, infra);
    double factor = 1.0;
    switch (unit) {
        case TimeUnit::Hour:  factor = 1.0 / HOURS_PER_MONTH; break;
        case TimeUnit::Day:   factor = HOURS_PER_DAY / HOURS_PER_MONTH; break;
        case TimeUnit::Month: factor = 1.0; break;
        case TimeUnit::Year:  factor = MONTHS_PER_YEAR; break;
    }
    return {monthly.provider,
            monthly.compute * factor,
            monthly.storage * factor,
            monthly.database * factor};
}

SimulationResult CostCalculator::simulate(const string& providerName,
                                          const vector<ResourceInput>& infra) const {
    SimulationResult result;
    result.provider = providerName;
    result.monthlyCost = {providerName, 0, 0, 0};

    for (const ProviderBundle& pb : providers) {
        if (pb.name != providerName) continue;
        for (const ResourceInput& r : infra) {
            ResourceCostDetail detail;
            detail.quantity = r.quantity;
            Specification s;

            if (r.type == "COMPUTE") {
                auto res = pb.factory->createCompute(r.vcpu, r.ramGB);
                detail.type = "COMPUTE";
                detail.productName = res->productName();
                s.vcpu = res->vcpu;
                s.ramGB = res->ramGB;
                detail.unitCostPerMonth = pb.adapter->getPrice(ResourceType::Compute, s, region);
                detail.totalCostPerMonth = r.quantity * detail.unitCostPerMonth;
                result.monthlyCost.compute += detail.totalCostPerMonth;
            } else if (r.type == "STORAGE") {
                auto res = pb.factory->createStorage(r.sizeGB);
                detail.type = "STORAGE";
                detail.productName = res->productName();
                detail.quantity = 1;
                s.sizeGB = res->sizeGB;
                detail.unitCostPerMonth = pb.adapter->getPrice(ResourceType::Storage, s, region);
                detail.totalCostPerMonth = detail.unitCostPerMonth;
                result.monthlyCost.storage += detail.totalCostPerMonth;
            } else if (r.type == "DATABASE") {
                auto res = pb.factory->createDatabase(r.engine);
                detail.type = "DATABASE";
                detail.productName = res->productName();
                detail.quantity = 1;
                s.engine = res->engine;
                detail.unitCostPerMonth = pb.adapter->getPrice(ResourceType::Database, s, region);
                detail.totalCostPerMonth = detail.unitCostPerMonth;
                result.monthlyCost.database += detail.totalCostPerMonth;
            }
            result.details.push_back(detail);
        }
    }
    return result;
}

vector<SimulationResult> CostCalculator::simulateAll(const vector<ResourceInput>& infra) const {
    vector<SimulationResult> results;
    for (const ProviderBundle& pb : providers) {
        results.push_back(simulate(pb.name, infra));
    }
    return results;
}

string CostCalculator::cheapestProvider(const vector<ResourceInput>& infra) const {
    auto all = calculateAll(infra);
    if (all.empty()) return "";
    auto best = min_element(all.begin(), all.end(),
        [](const ProviderCost& a, const ProviderCost& b) { return a.total() < b.total(); });
    return best->provider;
}

double CostCalculator::costDifference(const string& providerA, const string& providerB,
                                      const vector<ResourceInput>& infra) const {
    return totalFor(providerA, infra) - totalFor(providerB, infra);
}
