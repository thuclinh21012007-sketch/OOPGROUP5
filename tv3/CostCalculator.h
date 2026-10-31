#pragma once
// =====================================================================
// [TV3] CostCalculator.h - May tinh toan chi phi da dam may
// =====================================================================
#include "../common/Types.h"
#include "../tv1/CloudResourceFactory.h"
#include "../tv2/IPricingProviderAdapter.h"
#include "SimulationEngine.h"
#include <memory>
#include <string>
#include <vector>

using namespace std;

// Mot "bo" gom ten hang + Factory (TV1) + Adapter gia (TV2)
struct ProviderBundle {
    string name;
    unique_ptr<CloudResourceFactory> factory;
    unique_ptr<IPricingProviderAdapter> adapter;
};

class CostCalculator {
    vector<ProviderBundle> providers;
    Region region;

public:
    CostCalculator(vector<ProviderBundle> p, Region r);

    // Lay danh sach ten cac hang
    vector<string> getProviderNames() const;

    // Tinh chi phi / thang tren 1 hang
    ProviderCost costFor(const string& providerName, const vector<ResourceInput>& infra) const;

    // Tong chi phi / thang tren 1 hang
    double totalFor(const string& providerName, const vector<ResourceInput>& infra) const;

    // Tinh chi phi / thang cho tat ca cac hang
    vector<ProviderCost> calculateAll(const vector<ResourceInput>& infra) const;

    // Tinh chi phi theo don vi thoi gian tuy chon (gio, ngay, thang, nam)
    ProviderCost costForByTime(const string& providerName,
                               const vector<ResourceInput>& infra,
                               TimeUnit unit) const;

    // Mo phong chi tiet cho 1 hang
    SimulationResult simulate(const string& providerName,
                              const vector<ResourceInput>& infra) const;

    // Mo phong chi tiet cho tat ca cac hang
    vector<SimulationResult> simulateAll(const vector<ResourceInput>& infra) const;

    // Tim hang re nhat
    string cheapestProvider(const vector<ResourceInput>& infra) const;

    // Tinh chenh lech chi phi giua 2 hang (providerA - providerB)
    double costDifference(const string& providerA, const string& providerB,
                          const vector<ResourceInput>& infra) const;

    const Region& getRegion() const { return region; }
};
