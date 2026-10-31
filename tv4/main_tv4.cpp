// =====================================================================
// [TV4] main_tv4.cpp - Chuong trinh demo rieng cua Thanh vien 4
// Kiem tra Strategy Pattern voi 3 chien luoc toi uu chi phi
// =====================================================================
#include "OptimizationStrategy.h"
#include "CheapestProviderStrategy.h"
#include "RightSizingStrategy.h"
#include "PricingModelStrategy.h"
#include "../tv1/ConcreteFactories.h"
#include "../tv2/PricingAdapters.h"
#include "../tv3/CostCalculator.h"
#include <iomanip>
#include <iostream>
#include <memory>
#include <vector>

using namespace std;

int main() {
    cout << "======================================================\n";
    cout << "       DEMO THANH VIEN 4: STRATEGY PATTERN            \n";
    cout << "======================================================\n\n";

    // Khoi tao cac nha cung cap
    vector<ProviderBundle> providers;
    providers.push_back(ProviderBundle{"AWS",   make_unique<AWSResourceFactory>(),   make_unique<AwsPricingAdapter>()});
    providers.push_back(ProviderBundle{"GCP",   make_unique<GCPResourceFactory>(),   make_unique<GcpPricingAdapter>()});
    providers.push_back(ProviderBundle{"Azure", make_unique<AzureResourceFactory>(), make_unique<AzurePricingAdapter>()});

    CostCalculator calc(move(providers), DEFAULT_REGION);

    // Ha tang mau: 2 may 4vCPU/16GB chi dung 30% tai
    vector<ResourceInput> infra;
    ResourceInput vm; vm.type = "COMPUTE"; vm.quantity = 2; vm.vcpu = 4; vm.ramGB = 16; vm.usagePercent = 30;
    infra.push_back(vm);
    ResourceInput st; st.type = "STORAGE"; st.sizeGB = 500;
    infra.push_back(st);
    ResourceInput db; db.type = "DATABASE"; db.engine = "PostgreSQL";
    infra.push_back(db);

    OptimizationContext ctx{infra, calc, "AWS"};

    vector<unique_ptr<ICostOptimizationStrategy>> strategies;
    strategies.push_back(make_unique<CheapestProviderStrategy>());
    strategies.push_back(make_unique<RightSizingStrategy>());
    strategies.push_back(make_unique<PricingModelStrategy>());

    cout << "Chi phi ban dau tren AWS: $" << calc.totalFor("AWS", infra) << " USD/thang\n\n";
    cout << "=== KET QUA TU CAC CHIEN LUOC ===\n";
    cout << fixed << setprecision(2);

    for (const auto& strat : strategies) {
        auto recs = strat->optimize(ctx);
        for (const auto& r : recs) {
            cout << "- [" << r.strategy << "] " << r.description << "\n";
            cout << "  Truoc: $" << r.before << " -> Sau: $" << r.after
                      << " | Tiet kiem: $" << r.saving() << " (" << r.percent() << "%)\n\n";
        }
    }
    return 0;
}
