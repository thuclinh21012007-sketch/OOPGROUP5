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

using namespace std;

int main() {
    cout << "======================================================\n";
    cout << "    DEMO THANH VIEN 3: SIMULATION ENGINE & TESTS      \n";
    cout << "======================================================\n";

    // 1. Chay bo kiem thu tu dong
    TestSuite tests;
    tests.runAll();

    // 2. Demo mo phong chi tiet
    vector<ProviderBundle> providers;
    providers.push_back(ProviderBundle{"AWS",   make_unique<AWSResourceFactory>(),   make_unique<AwsPricingAdapter>()});
    providers.push_back(ProviderBundle{"GCP",   make_unique<GCPResourceFactory>(),   make_unique<GcpPricingAdapter>()});
    providers.push_back(ProviderBundle{"Azure", make_unique<AzureResourceFactory>(), make_unique<AzurePricingAdapter>()});

    CostCalculator calc(move(providers), DEFAULT_REGION);
    SimulationEngine engine(calc);

    vector<ResourceInput> infra;
    ResourceInput vm; vm.type = "COMPUTE"; vm.quantity = 2; vm.vcpu = 4; vm.ramGB = 16; vm.usagePercent = 35;
    infra.push_back(vm);
    ResourceInput st; st.type = "STORAGE"; st.sizeGB = 500;
    infra.push_back(st);
    ResourceInput db; db.type = "DATABASE"; db.engine = "PostgreSQL";
    infra.push_back(db);

    cout << "\n>>> KET QUA MO PHONG HA TANG MAU <<<\n";
    engine.printComparison(infra, TimeUnit::Month);
    engine.printDetail("AWS", infra);

    cout << "\nNha cung cap toi uu nhat: " << engine.findCheapest(infra) << "\n";
    return 0;
}
