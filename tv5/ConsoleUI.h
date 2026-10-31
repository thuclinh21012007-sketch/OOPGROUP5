#pragma once
// =====================================================================
// [TV5] ConsoleUI.h - Giao dien Console, Menu & Bao cao
// Thanh vien 5: UI, tich hop & bao cao
// =====================================================================
#include "../common/ICostService.h"
#include "../tv3/MultiCloudOptimizationService.h"
#include <vector>

using namespace std;

class ConsoleUI {
    ICostService& service;
    MultiCloudOptimizationService& fullService;
    vector<ResourceInput> infra;

    void showMenu() const;
    void addCompute();
    void addStorage();
    void addDatabase();
    void showInfra() const;
    void showCosts() const;
    void showDetailedCosts() const;
    void showRecommendations() const;
    void exportCsv() const;
    void runTests() const;

public:
    ConsoleUI(ICostService& s, MultiCloudOptimizationService& fs);
    void run();
};
