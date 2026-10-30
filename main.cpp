// =====================================================================
// main.cpp - TICH HOP HOAN CHINH HE THONG MO PHONG & TOI UU CHI PHI DA DAM MAY
//
// Du an gom 5 phan tu 5 thanh vien trong cac thu muc con:
// - common/ : Kieu du lieu & Interface hop dong dung chung
// - tv1/    : Kien truc & Domain Model (Abstract Factory Pattern)
// - tv2/    : Pricing API & Adapter Pattern
// - tv3/    : Simulation Engine & Cost Calculator + MultiCloudOptimizationService
// - tv4/    : Chien luoc toi uu chi phi (Strategy Pattern)
// - tv5/    : Giao dien Console, Bieu do ASCII & Xuat CSV
// =====================================================================
#include "common/Types.h"
#include "tv1/ConcreteFactories.h"
#include "tv2/PricingAdapters.h"
#include "tv3/CostCalculator.h"
#include "tv3/MultiCloudOptimizationService.h"
#include "tv4/CheapestProviderStrategy.h"
#include "tv4/RightSizingStrategy.h"
#include "tv4/PricingModelStrategy.h"
#include "tv5/ConsoleUI.h"
#include <memory>
#include <vector>

int main() {
    // 1. Khoi tao cac nha may tai nguyen (TV1) va cac adapter lay gia (TV2)
    std::vector<ProviderBundle> providers;
    providers.push_back(ProviderBundle{
        "AWS",
        std::make_unique<AWSResourceFactory>(),
        std::make_unique<AwsPricingAdapter>()
    });
    providers.push_back(ProviderBundle{
        "GCP",
        std::make_unique<GCPResourceFactory>(),
        std::make_unique<GcpPricingAdapter>()
    });
    providers.push_back(ProviderBundle{
        "Azure",
        std::make_unique<AzureResourceFactory>(),
        std::make_unique<AzurePricingAdapter>()
    });

    // 2. May tinh chi phi (TV3) nhan danh sach nha cung cap va vung mac dinh
    CostCalculator calculator(std::move(providers), DEFAULT_REGION);

    // 3. Dang ky cac chien luoc toi uu chi phi (TV4)
    std::vector<std::unique_ptr<ICostOptimizationStrategy>> strategies;
    strategies.push_back(std::make_unique<CheapestProviderStrategy>());
    strategies.push_back(std::make_unique<RightSizingStrategy>());
    strategies.push_back(std::make_unique<PricingModelStrategy>());

    // 4. Lop dieu phoi toan he thong (TV3) - Hang hien tai gia dinh la AWS
    MultiCloudOptimizationService service(std::move(calculator), std::move(strategies), "AWS");

    // 5. Khoi chay giao dien Console (TV5) thong qua ICostService
    ConsoleUI ui(service, service);
    ui.run();

    return 0;
}
