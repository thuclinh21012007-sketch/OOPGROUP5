#pragma once
// =====================================================================
// [TV4] OptimizationStrategy.h - Strategy Pattern cho toi uu chi phi
// Thanh vien 4: Toi uu chi phi
// =====================================================================
#include "../common/Types.h"
#include <string>
#include <vector>

using namespace std;

// Forward declaration
class CostCalculator;

// Context chua du lieu dau vao de chien luoc tinh toan
struct OptimizationContext {
    const vector<ResourceInput>& infra;  // ha tang hien tai
    const CostCalculator& calculator;         // may tinh de thu tinh lai voi cau hinh moi
    string currentProvider;              // hang dang dung hien tai
};

// Interface chung cho moi chien luoc toi uu
class ICostOptimizationStrategy {
public:
    virtual ~ICostOptimizationStrategy() = default;
    virtual string name() const = 0;
    virtual vector<Recommendation> optimize(const OptimizationContext& ctx) = 0;
};
