#include "PricingAdapters.h"

using namespace std;

StubPricingAdapter::StubPricingAdapter(string pName, StubRates r)
    : name(move(pName)), rates(r) {}

string StubPricingAdapter::providerName() const {
    return name;
}

double StubPricingAdapter::getPrice(ResourceType type, const Specification& specs, const Region& region) {
    (void)region; // Trong phien ban mo phong nay dung bang gia chuan hoa
    switch (type) {
        case ResourceType::Compute: {
            // Gia gio = baseRate + vcpu * perVcpuHour + ramGB * ramGB_rate (USD/gio)
            // Gia thang = Gia gio * HOURS_PER_MONTH (USD/thang)
            double hourly = rates.baseRate + specs.vcpu * rates.perVcpuHour + specs.ramGB * rates.ramGB;
            return hourly * HOURS_PER_MONTH;
        }
        case ResourceType::Storage:
            return specs.sizeGB * rates.perGbMonth;
        case ResourceType::Database:
            return rates.dbMonth;
    }
    return 0.0;
}

// AWS: base = 0.096$/h, vCPU = 0.048$/h, RAM = 0.006$/GB/h, Storage = 0.023$/GB/th, DB = 65$/th
AwsPricingAdapter::AwsPricingAdapter()
    : StubPricingAdapter("AWS", {0.096, 0.048, 0.006, 0.023, 65.0}) {}

// GCP: base = 0.082$/h, vCPU = 0.042$/h, RAM = 0.0054$/GB/h, Storage = 0.020$/GB/th, DB = 58$/th
GcpPricingAdapter::GcpPricingAdapter()
    : StubPricingAdapter("GCP", {0.082, 0.042, 0.0054, 0.020, 58.0}) {}

// Azure: base = 0.088$/h, vCPU = 0.044$/h, RAM = 0.0059$/GB/h, Storage = 0.021$/GB/th, DB = 62$/th
AzurePricingAdapter::AzurePricingAdapter()
    : StubPricingAdapter("Azure", {0.088, 0.044, 0.0059, 0.021, 62.0}) {}
