#include "TestSuite.h"
#include "../tv1/ConcreteFactories.h"
#include "../tv2/PricingAdapters.h"
#include <cmath>
#include <iostream>

void TestSuite::check(const std::string& testName, bool condition) {
    if (condition) {
        std::cout << "  [PASS] " << testName << "\n";
        ++passed;
    } else {
        std::cout << "  [FAIL] " << testName << "\n";
        ++failed;
    }
}

CostCalculator TestSuite::createCalculator() {
    std::vector<ProviderBundle> providers;
    providers.push_back(ProviderBundle{"AWS",   std::make_unique<AWSResourceFactory>(),   std::make_unique<AwsPricingAdapter>()});
    providers.push_back(ProviderBundle{"GCP",   std::make_unique<GCPResourceFactory>(),   std::make_unique<GcpPricingAdapter>()});
    providers.push_back(ProviderBundle{"Azure", std::make_unique<AzureResourceFactory>(), std::make_unique<AzurePricingAdapter>()});
    return CostCalculator(std::move(providers), DEFAULT_REGION);
}

std::vector<ResourceInput> TestSuite::sampleInfra() {
    std::vector<ResourceInput> infra;
    ResourceInput vm; vm.type = "COMPUTE"; vm.quantity = 2; vm.vcpu = 4; vm.ramGB = 16; vm.usagePercent = 30;
    infra.push_back(vm);
    ResourceInput st; st.type = "STORAGE"; st.sizeGB = 500;
    infra.push_back(st);
    ResourceInput db; db.type = "DATABASE"; db.engine = "PostgreSQL";
    infra.push_back(db);
    return infra;
}

void TestSuite::runAll() {
    std::cout << "\n========================================\n";
    std::cout << "       BO KIEM THU - THANH VIEN 3       \n";
    std::cout << "========================================\n";

    testCostCalculatorBasic();
    testCostByTimeUnit();
    testSimulationDetail();
    testCheapestProvider();
    testCostDifference();
    testEmptyInfra();
    testMultipleCompute();
    testAllProvidersReturned();

    std::cout << "\n----------------------------------------\n";
    std::cout << "Tong: " << (passed + failed) << " test | "
              << passed << " PASS | " << failed << " FAIL\n";
    std::cout << "========================================\n\n";
}

void TestSuite::testCostCalculatorBasic() {
    std::cout << "\n[Test] CostCalculator co ban\n";
    auto calc = createCalculator();
    auto infra = sampleInfra();

    // AWS: Compute = 2 * (4 * 0.050 * 730) = 292.0
    //      Storage = 500 * 0.100 = 50.0
    //      Database = 120.0
    //      Total = 462.0
    auto aws = calc.costFor("AWS", infra);
    check("AWS compute = 292.00", std::abs(aws.compute - 292.0) < 0.01);
    check("AWS storage = 50.00",  std::abs(aws.storage - 50.0) < 0.01);
    check("AWS database = 120.00", std::abs(aws.database - 120.0) < 0.01);
    check("AWS total = 462.00",   std::abs(aws.total() - 462.0) < 0.01);

    // GCP: Compute = 2 * (4 * 0.042 * 730) = 245.28
    auto gcp = calc.costFor("GCP", infra);
    check("GCP compute = 245.28", std::abs(gcp.compute - 245.28) < 0.01);

    // Azure: Compute = 2 * (4 * 0.048 * 730) = 280.32
    auto azure = calc.costFor("Azure", infra);
    check("Azure compute = 280.32", std::abs(azure.compute - 280.32) < 0.01);
}

void TestSuite::testCostByTimeUnit() {
    std::cout << "\n[Test] Tinh chi phi theo gio/ngay/thang/nam\n";
    auto calc = createCalculator();
    auto infra = sampleInfra();

    auto monthly = calc.costFor("AWS", infra);
    auto hourly  = calc.costForByTime("AWS", infra, TimeUnit::Hour);
    auto daily   = calc.costForByTime("AWS", infra, TimeUnit::Day);
    auto yearly  = calc.costForByTime("AWS", infra, TimeUnit::Year);

    check("Hourly = Monthly / 730", std::abs(hourly.total() - monthly.total() / 730.0) < 0.001);
    check("Daily = Monthly * 24/730", std::abs(daily.total() - monthly.total() * 24.0 / 730.0) < 0.001);
    check("Yearly = Monthly * 12", std::abs(yearly.total() - monthly.total() * 12.0) < 0.01);
}

void TestSuite::testSimulationDetail() {
    std::cout << "\n[Test] Mo phong chi tiet (SimulationResult)\n";
    auto calc = createCalculator();
    auto infra = sampleInfra();

    auto result = calc.simulate("AWS", infra);
    check("Provider = AWS", result.provider == "AWS");
    check("Co 3 dong chi tiet", result.details.size() == 3);
    check("Chi tiet dau tien la COMPUTE", result.details[0].type == "COMPUTE");
    check("San pham = AWS EC2", result.details[0].productName == "AWS EC2");
    check("So luong Compute = 2", result.details[0].quantity == 2);
    check("Tong thang phu hop", std::abs(result.monthlyCost.total() - 462.0) < 0.01);
}

void TestSuite::testCheapestProvider() {
    std::cout << "\n[Test] Tim nha cung cap re nhat\n";
    auto calc = createCalculator();
    auto infra = sampleInfra();

    std::string cheapest = calc.cheapestProvider(infra);
    check("Nha cung cap re nhat la GCP", cheapest == "GCP");
}

void TestSuite::testCostDifference() {
    std::cout << "\n[Test] Chenh lech chi phi giua 2 hang\n";
    auto calc = createCalculator();
    auto infra = sampleInfra();

    double diff = calc.costDifference("AWS", "GCP", infra);
    check("AWS dat hon GCP (diff > 0)", diff > 0);

    double awsTotal = calc.totalFor("AWS", infra);
    double gcpTotal = calc.totalFor("GCP", infra);
    check("Chenh lech = AWS - GCP", std::abs(diff - (awsTotal - gcpTotal)) < 0.001);
}

void TestSuite::testEmptyInfra() {
    std::cout << "\n[Test] Ha tang rong\n";
    auto calc = createCalculator();
    std::vector<ResourceInput> empty;

    auto cost = calc.costFor("AWS", empty);
    check("Chi phi AWS = 0 khi khong co tai nguyen", cost.total() == 0.0);
    check("calculateAll tra ve 3 hang", calc.calculateAll(empty).size() == 3);
}

void TestSuite::testMultipleCompute() {
    std::cout << "\n[Test] Nhieu may chu khac cau hinh\n";
    auto calc = createCalculator();
    std::vector<ResourceInput> infra;

    ResourceInput vm1; vm1.type = "COMPUTE"; vm1.quantity = 1; vm1.vcpu = 2; vm1.ramGB = 4;
    ResourceInput vm2; vm2.type = "COMPUTE"; vm2.quantity = 3; vm2.vcpu = 8; vm2.ramGB = 32;
    infra.push_back(vm1);
    infra.push_back(vm2);

    auto aws = calc.costFor("AWS", infra);
    check("Nhieu may: compute = 949.00", std::abs(aws.compute - 949.0) < 0.01);
}

void TestSuite::testAllProvidersReturned() {
    std::cout << "\n[Test] Tat ca nha cung cap deu tra ve ket qua\n";
    auto calc = createCalculator();
    auto infra = sampleInfra();

    auto all = calc.simulateAll(infra);
    check("simulateAll tra ve 3 ket qua", all.size() == 3);
    check("Ket qua 1 = AWS", all[0].provider == "AWS");
    check("Ket qua 2 = GCP", all[1].provider == "GCP");
    check("Ket qua 3 = Azure", all[2].provider == "Azure");
}
