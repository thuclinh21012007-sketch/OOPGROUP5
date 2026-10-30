#include "SimulationEngine.h"
#include "CostCalculator.h"
#include <iomanip>
#include <iostream>

double SimulationResult::costByTime(TimeUnit unit) const {
    double monthly = monthlyCost.total();
    switch (unit) {
        case TimeUnit::Hour:  return monthly / HOURS_PER_MONTH;
        case TimeUnit::Day:   return monthly / (HOURS_PER_MONTH / HOURS_PER_DAY);
        case TimeUnit::Month: return monthly;
        case TimeUnit::Year:  return monthly * MONTHS_PER_YEAR;
    }
    return monthly;
}

double SimulationResult::computeByTime(TimeUnit unit) const {
    switch (unit) {
        case TimeUnit::Hour:  return monthlyCost.compute / HOURS_PER_MONTH;
        case TimeUnit::Day:   return monthlyCost.compute / (HOURS_PER_MONTH / HOURS_PER_DAY);
        case TimeUnit::Month: return monthlyCost.compute;
        case TimeUnit::Year:  return monthlyCost.compute * MONTHS_PER_YEAR;
    }
    return monthlyCost.compute;
}

double SimulationResult::storageByTime(TimeUnit unit) const {
    switch (unit) {
        case TimeUnit::Hour:  return monthlyCost.storage / HOURS_PER_MONTH;
        case TimeUnit::Day:   return monthlyCost.storage / (HOURS_PER_MONTH / HOURS_PER_DAY);
        case TimeUnit::Month: return monthlyCost.storage;
        case TimeUnit::Year:  return monthlyCost.storage * MONTHS_PER_YEAR;
    }
    return monthlyCost.storage;
}

double SimulationResult::databaseByTime(TimeUnit unit) const {
    switch (unit) {
        case TimeUnit::Hour:  return monthlyCost.database / HOURS_PER_MONTH;
        case TimeUnit::Day:   return monthlyCost.database / (HOURS_PER_MONTH / HOURS_PER_DAY);
        case TimeUnit::Month: return monthlyCost.database;
        case TimeUnit::Year:  return monthlyCost.database * MONTHS_PER_YEAR;
    }
    return monthlyCost.database;
}

SimulationEngine::SimulationEngine(const CostCalculator& calc)
    : calculator(calc) {}

std::vector<SimulationResult> SimulationEngine::run(const std::vector<ResourceInput>& infra) const {
    return calculator.simulateAll(infra);
}

void SimulationEngine::printComparison(const std::vector<ResourceInput>& infra, TimeUnit unit) const {
    auto results = calculator.simulateAll(infra);
    std::string unitStr = timeUnitToString(unit);

    std::cout << "\n============ SO SANH CHI PHI (/" << unitStr << ") ============\n";
    std::cout << std::left << std::setw(10) << "Hang"
              << std::right << std::setw(12) << "Compute"
              << std::setw(12) << "Storage"
              << std::setw(12) << "Database"
              << std::setw(14) << "TONG" << "\n";
    std::cout << std::string(60, '-') << "\n";
    std::cout << std::fixed << std::setprecision(2);

    for (const auto& r : results) {
        std::cout << std::left << std::setw(10) << r.provider
                  << std::right
                  << std::setw(12) << r.computeByTime(unit)
                  << std::setw(12) << r.storageByTime(unit)
                  << std::setw(12) << r.databaseByTime(unit)
                  << std::setw(14) << r.costByTime(unit) << "\n";
    }
    std::cout << "(Don vi: USD/" << unitStr << ", gia mo phong - khong phai bao gia cam ket)\n";
}

void SimulationEngine::printDetail(const std::string& providerName,
                                   const std::vector<ResourceInput>& infra) const {
    auto result = calculator.simulate(providerName, infra);
    std::cout << "\n===== CHI TIET CHI PHI: " << providerName << " =====\n";
    std::cout << std::fixed << std::setprecision(2);
    std::cout << std::left << std::setw(6) << "STT"
              << std::setw(12) << "Loai"
              << std::setw(22) << "San pham"
              << std::right << std::setw(6) << "SL"
              << std::setw(14) << "Don gia/th"
              << std::setw(14) << "Tong/th" << "\n";
    std::cout << std::string(74, '-') << "\n";
    int idx = 1;
    for (const auto& d : result.details) {
        std::cout << std::left << std::setw(6) << idx++
                  << std::setw(12) << d.type
                  << std::setw(22) << d.productName
                  << std::right << std::setw(6) << d.quantity
                  << std::setw(14) << d.unitCostPerMonth
                  << std::setw(14) << d.totalCostPerMonth << "\n";
    }
    std::cout << std::string(74, '-') << "\n";
    std::cout << std::left << std::setw(46) << "TONG CONG"
              << std::right << std::setw(14) << "" << std::setw(14) << result.monthlyCost.total() << "\n";

    std::cout << "\nQuy doi thoi gian:\n";
    TimeUnit units[] = {TimeUnit::Hour, TimeUnit::Day, TimeUnit::Month, TimeUnit::Year};
    for (auto u : units) {
        std::cout << "  " << std::left << std::setw(10) << ("/" + timeUnitToString(u) + ":")
                  << std::right << std::setw(12) << result.costByTime(u) << " USD\n";
    }
}

std::string SimulationEngine::findCheapest(const std::vector<ResourceInput>& infra) const {
    return calculator.cheapestProvider(infra);
}
