// =====================================================================
// [TV5] main_tv5.cpp - Chuong trinh demo rieng cua Thanh vien 5
// Kiem tra giao dien Console & Menu tuong tac
// =====================================================================
#include "ConsoleUI.h"
#include "../tv1/ConcreteFactories.h"
#include "../tv2/PricingAdapters.h"
#include "../tv3/CostCalculator.h"
#include "../tv3/MultiCloudOptimizationService.h"
#include "../tv4/CheapestProviderStrategy.h"
#include "../tv4/RightSizingStrategy.h"
#include "../tv4/PricingModelStrategy.h"
#include <memory>
#include <vector>

int main() {
    std::vector<ProviderBundle> providers;
    providers.push_back(ProviderBundle{"AWS",   std::make_unique<AWSResourceFactory>(),   std::make_unique<AwsPricingAdapter>()});
    providers.push_back(ProviderBundle{"GCP",   std::make_unique<GCPResourceFactory>(),   std::make_unique<GcpPricingAdapter>()});
    providers.push_back(ProviderBundle{"Azure", std::make_unique<AzureResourceFactory>(), std::make_unique<AzurePricingAdapter>()});

    CostCalculator calculator(std::move(providers), DEFAULT_REGION);

    std::vector<std::unique_ptr<ICostOptimizationStrategy>> strategies;
    strategies.push_back(std::make_unique<CheapestProviderStrategy>());
    strategies.push_back(std::make_unique<RightSizingStrategy>());
    strategies.push_back(std::make_unique<PricingModelStrategy>());

    MultiCloudOptimizationService service(std::move(calculator), std::move(strategies), "AWS");

    ConsoleUI ui(service, service);
    ui.run();
    return 0;
}
