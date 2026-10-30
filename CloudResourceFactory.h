#pragma once
// =====================================================================
// [TV1] CloudResourceFactory.h - Abstract Factory Interface
// Cung cap giao dien tao mot ho san pham tai nguyen dam may
// =====================================================================
#include "CloudResource.h"
#include <memory>
#include <string>

class CloudResourceFactory {
public:
    virtual ~CloudResourceFactory() = default;
    virtual std::unique_ptr<ComputeResource>  createCompute(int vcpu, int ramGB) = 0;
    virtual std::unique_ptr<StorageResource>  createStorage(int sizeGB) = 0;
    virtual std::unique_ptr<DatabaseResource> createDatabase(const std::string& engine) = 0;
};
