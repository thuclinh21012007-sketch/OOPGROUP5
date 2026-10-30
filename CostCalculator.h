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

// Mot "bo" gom ten hang + Factory (TV1) + Adapter gia (TV2)
struct ProviderBundle {
    std::string name;
    std::unique_ptr<CloudResourceFactory> factory;
    std::unique_ptr<IPricingProviderAdapter> adapter;
};

class CostCalculator {
    std::vector<ProviderBundle> providers;
    Region region;

public:
    CostCalculator(std::vector<ProviderBundle> p, Region r);

    // Lay danh sach ten cac hang
    std::vector<std::string> getProviderNames() const;

    // Tinh chi phi / thang tren 1 hang
    ProviderCost costFor(const std::string& providerName, const std::vector<ResourceInput>& infra) const;

    // Tong chi phi / thang tren 1 hang
    double totalFor(const std::string& providerName, const std::vector<ResourceInput>& infra) const;

    // Tinh chi phi / thang cho tat ca cac hang
    std::vector<ProviderCost> calculateAll(const std::vector<ResourceInput>& infra) const;

    // Tinh chi phi theo don vi thoi gian tuy chon (gio, ngay, thang, nam)
    ProviderCost costForByTime(const std::string& providerName,
                               const std::vector<ResourceInput>& infra,
                               TimeUnit unit) const;

    // Mo phong chi tiet cho 1 hang
    SimulationResult simulate(const std::string& providerName,
                              const std::vector<ResourceInput>& infra) const;

    // Mo phong chi tiet cho tat ca cac hang
    std::vector<SimulationResult> simulateAll(const std::vector<ResourceInput>& infra) const;

    // Tim hang re nhat
    std::string cheapestProvider(const std::vector<ResourceInput>& infra) const;

    // Tinh chenh lech chi phi giua 2 hang (providerA - providerB)
    double costDifference(const std::string& providerA, const std::string& providerB,
                          const std::vector<ResourceInput>& infra) const;

    const Region& getRegion() const { return region; }
};
