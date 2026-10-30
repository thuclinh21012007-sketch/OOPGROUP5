#pragma once
// =====================================================================
// [CHUNG] ICostService.h - Interface hop dong giua UI va Backend
// UI chi phu thuoc vao interface nay, khong phu thuoc vao logic ben duoi.
// =====================================================================
#include "Types.h"
#include <vector>

class ICostService {
public:
    virtual ~ICostService() = default;
    virtual std::vector<ProviderCost> calculate(const std::vector<ResourceInput>& infra) = 0;
    virtual std::vector<Recommendation> recommend(const std::vector<ResourceInput>& infra) = 0;
};
