#pragma once
// =====================================================================
// [TV1] ConcreteResources.h - 9 san pham cu the cua AWS, GCP, Azure
// =====================================================================
#include "CloudResource.h"

// --- San pham cu the AWS ---
class EC2Instance : public ComputeResource {
public:
    using ComputeResource::ComputeResource;
    std::string productName() const override { return "AWS EC2"; }
};

class S3Bucket : public StorageResource {
public:
    using StorageResource::StorageResource;
    std::string productName() const override { return "AWS S3"; }
};

class RDSInstance : public DatabaseResource {
public:
    using DatabaseResource::DatabaseResource;
    std::string productName() const override { return "AWS RDS"; }
};

// --- San pham cu the GCP ---
class ComputeEngine : public ComputeResource {
public:
    using ComputeResource::ComputeResource;
    std::string productName() const override { return "GCP Compute Engine"; }
};

class CloudStorage : public StorageResource {
public:
    using StorageResource::StorageResource;
    std::string productName() const override { return "GCP Cloud Storage"; }
};

class CloudSQL : public DatabaseResource {
public:
    using DatabaseResource::DatabaseResource;
    std::string productName() const override { return "GCP Cloud SQL"; }
};

// --- San pham cu the Azure ---
class AzureVM : public ComputeResource {
public:
    using ComputeResource::ComputeResource;
    std::string productName() const override { return "Azure Virtual Machine"; }
};

class AzureBlob : public StorageResource {
public:
    using StorageResource::StorageResource;
    std::string productName() const override { return "Azure Blob Storage"; }
};

class AzureDatabase : public DatabaseResource {
public:
    using DatabaseResource::DatabaseResource;
    std::string productName() const override { return "Azure Database"; }
};
