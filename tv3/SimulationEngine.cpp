#include "SimulationEngine.h"
#include "CostCalculator.h"
#include <iomanip>
#include <iostream>

using namespace std;

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

vector<SimulationResult> SimulationEngine::run(const vector<ResourceInput>& infra) const {
    return calculator.simulateAll(infra);
}

void SimulationEngine::printComparison(const vector<ResourceInput>& infra, TimeUnit unit) const {
    auto results = calculator.simulateAll(infra);
    string unitStr = timeUnitToString(unit);

    cout << "\n============ SO SANH CHI PHI (/" << unitStr << ") ============\n";
    cout << left << setw(10) << "Hang"
              << right << setw(12) << "Compute"
              << setw(12) << "Storage"
              << setw(12) << "Database"
              << setw(14) << "TONG" << "\n";
    cout << string(60, '-') << "\n";
    cout << fixed << setprecision(2);

    for (const auto& r : results) {
        cout << left << setw(10) << r.provider
                  << right
                  << setw(12) << r.computeByTime(unit)
                  << setw(12) << r.storageByTime(unit)
                  << setw(12) << r.databaseByTime(unit)
                  << setw(14) << r.costByTime(unit) << "\n";
    }
    cout << "(Don vi: USD/" << unitStr << ", gia mo phong - khong phai bao gia cam ket)\n";
}

void SimulationEngine::printDetail(const string& providerName,
                                   const vector<ResourceInput>& infra) const {
    auto result = calculator.simulate(providerName, infra);
    cout << "\n===== CHI TIET CHI PHI: " << providerName << " =====\n";
    cout << fixed << setprecision(2);
    cout << left << setw(6) << "STT"
              << setw(12) << "Loai"
              << setw(22) << "San pham"
              << right << setw(6) << "SL"
              << setw(14) << "Don gia/th"
              << setw(14) << "Tong/th" << "\n";
    cout << string(74, '-') << "\n";
    int idx = 1;
    for (const auto& d : result.details) {
        cout << left << setw(6) << idx++
                  << setw(12) << d.type
                  << setw(22) << d.productName
                  << right << setw(6) << d.quantity
                  << setw(14) << d.unitCostPerMonth
                  << setw(14) << d.totalCostPerMonth << "\n";
    }
    cout << string(74, '-') << "\n";
    cout << left << setw(46) << "TONG CONG"
              << right << setw(14) << "" << setw(14) << result.monthlyCost.total() << "\n";

    cout << "\nQuy doi thoi gian:\n";
    TimeUnit units[] = {TimeUnit::Hour, TimeUnit::Day, TimeUnit::Month, TimeUnit::Year};
    for (auto u : units) {
        cout << "  " << left << setw(10) << ("/" + timeUnitToString(u) + ":")
                  << right << setw(12) << result.costByTime(u) << " USD\n";
    }
}

string SimulationEngine::findCheapest(const vector<ResourceInput>& infra) const {
    return calculator.cheapestProvider(infra);
}
