#pragma once
// =====================================================================
// [TV2] IPricingProviderAdapter.h - Adapter Interface lay gia
// Thanh vien 2: Pricing API & Adapter
// Hop dong: tra ve don gia THEO THANG (USD) cho 1 don vi tai nguyen
// =====================================================================
#include "../common/Types.h"
#include <string>

using namespace std;

class IPricingProviderAdapter {
public:
    virtual ~IPricingProviderAdapter() = default;
    virtual string providerName() const = 0;
    virtual double getPrice(ResourceType type, const Specification& specs, const Region& region) = 0;
};
