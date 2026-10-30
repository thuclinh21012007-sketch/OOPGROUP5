#pragma once
// =====================================================================
// [TV2] PricingAdapters.h - Cac lop Adapter cu the cho AWS, GCP, Azure
// Chuan hoa gia ve USD/thang
// =====================================================================
#include "IPricingProviderAdapter.h"
#include <string>

// Bang gia gia dinh, don vi chuan hoa
struct StubRates {
    double perVcpuHour; // USD / vCPU / gio
    double perGbMonth;  // USD / GB / thang
    double dbMonth;     // USD / database / thang
};

class StubPricingAdapter : public IPricingProviderAdapter {
protected:
    std::string name;
    StubRates rates;
public:
    StubPricingAdapter(std::string pName, StubRates r);
    std::string providerName() const override;
    double getPrice(ResourceType type, const Specification& specs, const Region& region) override;
};

// 3 Adapter cu the cho 3 nha cung cap
class AwsPricingAdapter : public StubPricingAdapter {
public:
    AwsPricingAdapter();
};

class GcpPricingAdapter : public StubPricingAdapter {
public:
    GcpPricingAdapter();
};

class AzurePricingAdapter : public StubPricingAdapter {
public:
    AzurePricingAdapter();
};
