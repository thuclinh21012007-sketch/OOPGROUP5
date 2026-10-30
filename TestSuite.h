#pragma once
// =====================================================================
// [TV3] TestSuite.h - Bo kiem thu tu dong (25 test cases)
// Thanh vien 3: Kiem thu toan dien Simulation Engine & Cost Calculator
// =====================================================================
#include "CostCalculator.h"
#include <string>
#include <vector>

class TestSuite {
    int passed = 0;
    int failed = 0;

    void check(const std::string& testName, bool condition);
    CostCalculator createCalculator();
    std::vector<ResourceInput> sampleInfra();

    void testCostCalculatorBasic();
    void testCostByTimeUnit();
    void testSimulationDetail();
    void testCheapestProvider();
    void testCostDifference();
    void testEmptyInfra();
    void testMultipleCompute();
    void testAllProvidersReturned();

public:
    void runAll();
    int getPassed() const { return passed; }
    int getFailed() const { return failed; }
};
