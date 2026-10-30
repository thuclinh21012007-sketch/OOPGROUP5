#pragma once
// =====================================================================
// [TV1] CloudResource.h - Domain Model cho cac tai nguyen dam may
// Thanh vien 1: Kien truc & Domain Model
// =====================================================================
#include <string>
#include <iostream>

// San pham truu tuong goc cho moi tai nguyen
class CloudResource {
public:
    virtual ~CloudResource() = default;
    virtual std::string productName() const = 0;
    virtual void display() const {
        std::cout << "San pham: " << productName() << std::endl;
    }
};

// May chu ao (Compute)
class ComputeResource : public CloudResource {
public:
    int vcpu;
    int ramGB;
    ComputeResource(int v, int r) : vcpu(v), ramGB(r) {}
    void display() const override {
        std::cout << "[Compute] " << productName() 
                  << " | vCPU: " << vcpu 
                  << " | RAM: " << ramGB << "GB\n";
    }
};

// Kho luu tru (Storage)
class StorageResource : public CloudResource {
public:
    int sizeGB;
    explicit StorageResource(int s) : sizeGB(s) {}
    void display() const override {
        std::cout << "[Storage] " << productName() 
                  << " | Dung luong: " << sizeGB << "GB\n";
    }
};

// Co so du lieu (Database)
class DatabaseResource : public CloudResource {
public:
    std::string engine;
    explicit DatabaseResource(std::string e) : engine(std::move(e)) {}
    void display() const override {
        std::cout << "[Database] " << productName() 
                  << " | Engine: " << engine << "\n";
    }
};
