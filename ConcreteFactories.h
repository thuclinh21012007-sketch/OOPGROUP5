#pragma once
// =====================================================================
// [TV1] ConcreteFactories.h - 3 Concrete Factory cho AWS, GCP, Azure
// =====================================================================
#include "CloudResourceFactory.h"
#include "ConcreteResources.h"

// --- Factory cho AWS ---
class AWSResourceFactory : public CloudResourceFactory {
public:
    std::unique_ptr<ComputeResource> createCompute(int vcpu, int ramGB) override {
        return std::make_unique<EC2Instance>(vcpu, ramGB);
    }
    std::unique_ptr<StorageResource> createStorage(int sizeGB) override {
        return std::make_unique<S3Bucket>(sizeGB);
    }
    std::unique_ptr<DatabaseResource> createDatabase(const std::string& engine) override {
        return std::make_unique<RDSInstance>(engine);
    }
};

// --- Factory cho GCP ---
class GCPResourceFactory : public CloudResourceFactory {
public:
    std::unique_ptr<ComputeResource> createCompute(int vcpu, int ramGB) override {
        return std::make_unique<ComputeEngine>(vcpu, ramGB);
    }
    std::unique_ptr<StorageResource> createStorage(int sizeGB) override {
        return std::make_unique<CloudStorage>(sizeGB);
    }
    std::unique_ptr<DatabaseResource> createDatabase(const std::string& engine) override {
        return std::make_unique<CloudSQL>(engine);
    }
};

// --- Factory cho Azure ---
class AzureResourceFactory : public CloudResourceFactory {
public:
    std::unique_ptr<ComputeResource> createCompute(int vcpu, int ramGB) override {
        return std::make_unique<AzureVM>(vcpu, ramGB);
    }
    std::unique_ptr<StorageResource> createStorage(int sizeGB) override {
        return std::make_unique<AzureBlob>(sizeGB);
    }
    std::unique_ptr<DatabaseResource> createDatabase(const std::string& engine) override {
        return std::make_unique<AzureDatabase>(engine);
    }
};
