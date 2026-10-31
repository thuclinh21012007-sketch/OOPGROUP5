#pragma once
// =====================================================================
// [TV1] CloudResource.h - Domain Model cho cac tai nguyen dam may
// Thanh vien 1: Kien truc & Domain Model
// =====================================================================
#include <string>
#include <iostream>

using namespace std;

// San pham truu tuong goc cho moi tai nguyen
class CloudResource {
public:
    virtual ~CloudResource() = default;
    virtual string productName() const = 0;
    virtual void display() const {
        cout << "San pham: " << productName() << endl;
    }
};

// May chu ao (Compute)
class ComputeResource : public CloudResource {
public:
    int vcpu;
    int ramGB;
    ComputeResource(int v, int r) : vcpu(v), ramGB(r) {}
    void display() const override {
        cout << "[Compute] " << productName() 
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
        cout << "[Storage] " << productName() 
                  << " | Dung luong: " << sizeGB << "GB\n";
    }
};

// Co so du lieu (Database)
class DatabaseResource : public CloudResource {
public:
    string engine;
    explicit DatabaseResource(string e) : engine(move(e)) {}
    void display() const override {
        cout << "[Database] " << productName() 
                  << " | Engine: " << engine << "\n";
    }
};
