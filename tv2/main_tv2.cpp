// =====================================================================
// [TV2] main_tv2.cpp - Chuong trinh demo rieng cua Thanh vien 2
// Kiem tra Pricing Adapters cho AWS, GCP, Azure
// =====================================================================
#include "PricingAdapters.h"
#include <iomanip>
#include <iostream>
#include <memory>
#include <vector>

int main() {
    std::cout << "========================================\n";
    std::cout << "    DEMO THANH VIEN 2: PRICING ADAPTER  \n";
    std::cout << "========================================\n\n";

    std::vector<std::unique_ptr<IPricingProviderAdapter>> adapters;
    adapters.push_back(std::make_unique<AwsPricingAdapter>());
    adapters.push_back(std::make_unique<GcpPricingAdapter>());
    adapters.push_back(std::make_unique<AzurePricingAdapter>());

    Specification vmSpec{4, 16, 0, ""};
    Specification stSpec{0, 0, 500, ""};
    Specification dbSpec{0, 0, 0, "PostgreSQL"};

    std::cout << std::left << std::setw(10) << "Hang"
              << std::right << std::setw(18) << "Compute (4vCPU)"
              << std::setw(18) << "Storage (500GB)"
              << std::setw(18) << "DB (PostgreSQL)" << "\n";
    std::cout << std::string(64, '-') << "\n";
    std::cout << std::fixed << std::setprecision(2);

    for (const auto& a : adapters) {
        double c = a->getPrice(ResourceType::Compute, vmSpec, DEFAULT_REGION);
        double s = a->getPrice(ResourceType::Storage, stSpec, DEFAULT_REGION);
        double d = a->getPrice(ResourceType::Database, dbSpec, DEFAULT_REGION);

        std::cout << std::left << std::setw(10) << a->providerName()
                  << std::right << std::setw(18) << c
                  << std::setw(18) << s
                  << std::setw(18) << d << "\n";
    }
    std::cout << "(Don vi: USD/thang - gia mo phong)\n";
    return 0;
}
