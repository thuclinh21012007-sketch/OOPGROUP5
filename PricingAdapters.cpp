#include "PricingAdapters.h"

StubPricingAdapter::StubPricingAdapter(std::string pName, StubRates r)
    : name(std::move(pName)), rates(r) {}

std::string StubPricingAdapter::providerName() const {
    return name;
}

double StubPricingAdapter::getPrice(ResourceType type, const Specification& specs, const Region& region) {
    (void)region; // Trong phien ban mo phong nay dung bang gia chuan hoa
    switch (type) {
        case ResourceType::Compute:
            return specs.vcpu * rates.perVcpuHour * HOURS_PER_MONTH;
        case ResourceType::Storage:
            return specs.sizeGB * rates.perGbMonth;
        case ResourceType::Database:
            return rates.dbMonth;
    }
    return 0.0;
}

// AWS: vCPU = 0.050$/h, Storage = 0.100$/GB/th, DB = 120$/th
AwsPricingAdapter::AwsPricingAdapter()
    : StubPricingAdapter("AWS", {0.050, 0.100, 120.0}) {}

// GCP: vCPU = 0.042$/h, Storage = 0.085$/GB/th, DB = 115$/th
GcpPricingAdapter::GcpPricingAdapter()
    : StubPricingAdapter("GCP", {0.042, 0.085, 115.0}) {}

// Azure: vCPU = 0.048$/h, Storage = 0.092$/GB/th, DB = 125$/th
AzurePricingAdapter::AzurePricingAdapter()
    : StubPricingAdapter("Azure", {0.048, 0.092, 125.0}) {}
