// =====================================================================
// [TV2] main_tv2.cpp - Chuong trinh demo rieng cua Thanh vien 2
// Kiem tra Pricing Adapters cho AWS, GCP, Azure
// =====================================================================
#include "PricingAdapters.h"
#include <iomanip>
#include <iostream>
#include <memory>
#include <vector>

using namespace std;

int main() {
    cout << "========================================\n";
    cout << "    DEMO THANH VIEN 2: PRICING ADAPTER  \n";
    cout << "========================================\n\n";

    vector<unique_ptr<IPricingProviderAdapter>> adapters;
    adapters.push_back(make_unique<AwsPricingAdapter>());
    adapters.push_back(make_unique<GcpPricingAdapter>());
    adapters.push_back(make_unique<AzurePricingAdapter>());

    Specification vmSpec{4, 16, 0, ""};
    Specification stSpec{0, 0, 500, ""};
    Specification dbSpec{0, 0, 0, "PostgreSQL"};

    cout << left << setw(10) << "Hang"
              << right << setw(18) << "Compute (4vCPU)"
              << setw(18) << "Storage (500GB)"
              << setw(18) << "DB (PostgreSQL)" << "\n";
    cout << string(64, '-') << "\n";
    cout << fixed << setprecision(2);

    for (const auto& a : adapters) {
        double c = a->getPrice(ResourceType::Compute, vmSpec, DEFAULT_REGION);
        double s = a->getPrice(ResourceType::Storage, stSpec, DEFAULT_REGION);
        double d = a->getPrice(ResourceType::Database, dbSpec, DEFAULT_REGION);

        cout << left << setw(10) << a->providerName()
                  << right << setw(18) << c
                  << setw(18) << s
                  << setw(18) << d << "\n";
    }
    cout << "(Don vi: USD/thang - gia mo phong)\n";
    return 0;
}
