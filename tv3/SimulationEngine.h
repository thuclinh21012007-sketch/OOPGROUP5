#pragma once
// =====================================================================
// [TV3] SimulationEngine.h - Cau truc ket qua & lop dieu phoi mo phong
// Thanh vien 3: Simulation Engine & Cost Calculator
// =====================================================================
#include "../common/Types.h"
#include <string>
#include <vector>

using namespace std;

// Chi tiet chi phi cho tung tai nguyen don le
struct ResourceCostDetail {
    string type;         // "COMPUTE" / "STORAGE" / "DATABASE"
    string productName;  // ten san pham theo hang (EC2, S3, RDS...)
    int quantity = 0;         // so luong
    double unitCostPerMonth = 0.0;  // gia 1 don vi / thang
    double totalCostPerMonth = 0.0; // gia tong / thang (quantity * unitCost)
};

// Ket qua mo phong chi tiet cho 1 nha cung cap
struct SimulationResult {
    string provider;
    vector<ResourceCostDetail> details;
    ProviderCost monthlyCost;

    // Chuyen doi tong chi phi theo don vi thoi gian
    double costByTime(TimeUnit unit) const;
    double computeByTime(TimeUnit unit) const;
    double storageByTime(TimeUnit unit) const;
    double databaseByTime(TimeUnit unit) const;
};

// Forward declaration
class CostCalculator;

// SimulationEngine: dieu phoi cac kich ban mo phong va xuat ket qua
class SimulationEngine {
    const CostCalculator& calculator;
public:
    explicit SimulationEngine(const CostCalculator& calc);

    // Mo phong cho tat ca cac nha cung cap
    vector<SimulationResult> run(const vector<ResourceInput>& infra) const;

    // In bang so sanh chi phi giua cac hang theo don vi thoi gian
    void printComparison(const vector<ResourceInput>& infra, TimeUnit unit = TimeUnit::Month) const;

    // In chi tiet tung loai tai nguyen cho 1 nha cung cap
    void printDetail(const string& providerName, const vector<ResourceInput>& infra) const;

    // Tim nha cung cap re nhat
    string findCheapest(const vector<ResourceInput>& infra) const;
};
