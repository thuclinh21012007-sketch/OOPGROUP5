#pragma once
// =====================================================================
// [TV1] ConcreteFactories.h - 3 Concrete Factory cho AWS, GCP, Azure
// =====================================================================
#include "CloudResourceFactory.h"
#include "ConcreteResources.h"

using namespace std;

// --- Factory cho AWS ---
class AWSResourceFactory : public CloudResourceFactory {
public:
    unique_ptr<ComputeResource> createCompute(int vcpu, int ramGB) override {
        return make_unique<EC2Instance>(vcpu, ramGB);
    }
    unique_ptr<StorageResource> createStorage(int sizeGB) override {
        return make_unique<S3Bucket>(sizeGB);
    }
    unique_ptr<DatabaseResource> createDatabase(const string& engine) override {
        return make_unique<RDSInstance>(engine);
    }
};

// --- Factory cho GCP ---
class GCPResourceFactory : public CloudResourceFactory {
public:
    unique_ptr<ComputeResource> createCompute(int vcpu, int ramGB) override {
        return make_unique<ComputeEngine>(vcpu, ramGB);
    }
    unique_ptr<StorageResource> createStorage(int sizeGB) override {
        return make_unique<CloudStorage>(sizeGB);
    }
    unique_ptr<DatabaseResource> createDatabase(const string& engine) override {
        return make_unique<CloudSQL>(engine);
    }
};

// --- Factory cho Azure ---
class AzureResourceFactory : public CloudResourceFactory {
public:
    unique_ptr<ComputeResource> createCompute(int vcpu, int ramGB) override {
        return make_unique<AzureVM>(vcpu, ramGB);
    }
    unique_ptr<StorageResource> createStorage(int sizeGB) override {
        return make_unique<AzureBlob>(sizeGB);
    }
    unique_ptr<DatabaseResource> createDatabase(const string& engine) override {
        return make_unique<AzureDatabase>(engine);
    }
};
