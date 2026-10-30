// =====================================================================
// [TV3] main_tv3.cpp - Chuong trinh demo rieng cua Thanh vien 3
// Chay Simulation Engine va Bo kiem thu tu dong (25 test cases)
// =====================================================================
#include "CostCalculator.h"
#include "SimulationEngine.h"
#include "TestSuite.h"
#include "../tv1/ConcreteFactories.h"
#include "../tv2/PricingAdapters.h"
#include <iostream>

int main() {
    std::cout << "======================================================\n";
    std::cout << "    DEMO THANH VIEN 3: SIMULATION ENGINE & TESTS      \n";
    std::cout << "======================================================\n";

    // 1. Chay bo kiem thu tu dong
    TestSuite tests;
    tests.runAll();

    // 2. Demo mo phong chi tiet
    std::vector<ProviderBundle> providers;
    providers.push_back(ProviderBundle{"AWS",   std::make_unique<AWSResourceFactory>(),   std::make_unique<AwsPricingAdapter>()});
    providers.push_back(ProviderBundle{"GCP",   std::make_unique<GCPResourceFactory>(),   std::make_unique<GcpPricingAdapter>()});
    providers.push_back(ProviderBundle{"Azure", std::make_unique<AzureResourceFactory>(), std::make_unique<AzurePricingAdapter>()});

    CostCalculator calc(std::move(providers), DEFAULT_REGION);
    SimulationEngine engine(calc);

    std::vector<ResourceInput> infra;
    ResourceInput vm; vm.type = "COMPUTE"; vm.quantity = 2; vm.vcpu = 4; vm.ramGB = 16; vm.usagePercent = 35;
    infra.push_back(vm);
    ResourceInput st; st.type = "STORAGE"; st.sizeGB = 500;
    infra.push_back(st);
    ResourceInput db; db.type = "DATABASE"; db.engine = "PostgreSQL";
    infra.push_back(db);

    std::cout << "\n>>> KET QUA MO PHONG HA TANG MAU <<<\n";
    engine.printComparison(infra, TimeUnit::Month);
    engine.printDetail("AWS", infra);

    std::cout << "\nNha cung cap toi uu nhat: " << engine.findCheapest(infra) << "\n";
    return 0;
}
