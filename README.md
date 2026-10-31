[README.md](https://github.com/user-attachments/files/34218294/README.md)
# DỰ ÁN LỚN: HỆ THỐNG MÔ PHỎNG & TỐI ƯU CHI PHÍ ĐA ĐÁM MÂY (MULTI-CLOUD COST OPTIMIZER)

Dự án lập trình hướng đối tượng (OOP) mô phỏng chi phí và đề xuất giải pháp tối ưu hóa chi phí đa hạ tầng đám mây (**AWS**, **GCP**, **Azure**) cho các tài nguyên **Compute**, **Storage**, và **Database**.

---

## 1. Cấu Trúc Thư Mục Phân Chia Theo 5 Thành Viên

Dự án được phân tách rõ ràng thành 5 module tương ứng với 5 thành viên nhóm:

```
Du an to/
├── common/                          # [CHUNG] Quy ước dữ liệu & Interface dùng chung
│   ├── Types.h                      # Structs: Specification, ResourceInput, ProviderCost, Recommendation...
│   └── ICostService.h               # Interface hợp đồng giữa Backend và UI
│
├── tv1/                             # [THÀNH VIÊN 1] Kiến trúc & Domain Model (Abstract Factory)
│   ├── CloudResource.h              # Base class trừu tượng: CloudResource, Compute, Storage, Database
│   ├── ConcreteResources.h          # 9 sản phẩm cụ thể: EC2, S3, RDS, ComputeEngine, CloudStorage...
│   ├── CloudResourceFactory.h       # Interface Abstract Factory: CloudResourceFactory
│   ├── ConcreteFactories.h          # 3 Factory cụ thể: AWSResourceFactory, GCPResourceFactory, Azure...
│   └── main_tv1.cpp                 # Demo độc lập của TV1
│
├── tv2/                             # [THÀNH VIÊN 2] Pricing API & Adapter Pattern
│   ├── IPricingProviderAdapter.h    # Interface Adapter chuẩn hóa đơn giá
│   ├── PricingAdapters.h            # Khai báo các Adapter bảng giá cho AWS, GCP, Azure
│   ├── PricingAdapters.cpp          # Triển khai tính giá theo giờ / GB / tháng
│   ├── data/                        # File JSON mock bảng giá mẫu
│   └── main_tv2.cpp                 # Demo độc lập của TV2
│
├── tv3/                             # [THÀNH VIÊN 3] Simulation Engine & Cost Calculator
│   ├── CostCalculator.h             # Máy tính toán chi phí chính cho tất cả các hãng
│   ├── CostCalculator.cpp           # Tính theo giờ/ngày/tháng/năm, so sánh chênh lệch
│   ├── SimulationEngine.h           # Điều phối mô phỏng, xuất bảng so sánh & chi tiết
│   ├── SimulationEngine.cpp         # Định dạng kết quả, quy đổi thời gian
│   ├── MultiCloudOptimizationService.h # Lớp điều phối trung tâm kết nối Calculator + Strategy + UI
│   ├── MultiCloudOptimizationService.cpp
│   ├── TestSuite.h                  # Bộ kiểm thử tự động 25 test cases
│   ├── TestSuite.cpp
│   └── main_tv3.cpp                 # Demo độc lập & chạy test suite của TV3
│
├── tv4/                             # [THÀNH VIÊN 4] Chiến lược Tối ưu chi phí (Strategy Pattern)
│   ├── OptimizationStrategy.h       # Interface Strategy: ICostOptimizationStrategy & Context
│   ├── CheapestProviderStrategy.h/.cpp # Chiến lược chuyển sang nhà cung cấp rẻ nhất
│   ├── RightSizingStrategy.h/.cpp   # Chiến lược hạ cấu hình máy chủ tải thấp (< 40%)
│   ├── PricingModelStrategy.h/.cpp  # Chiến lược cam kết sử dụng Reserved Plans (giảm 30%)
│   └── main_tv4.cpp                 # Demo độc lập của TV4
│
├── tv5/                             # [THÀNH VIÊN 5] Console UI, Tích hợp & Báo cáo
│   ├── ConsoleUI.h                  # Menu tương tác 10 chức năng (0-9)
│   ├── ConsoleUI.cpp                # Nhập hạ tầng, Dashboard biểu đồ ASCII, xuất CSV
│   └── main_tv5.cpp                 # Demo giao diện của TV5
│
├── main.cpp                         # Điểm khởi chạy chính tích hợp toàn bộ 5 thành viên
├── build.bat                        # Script biên dịch tự động 1-click trên Windows
├── CMakeLists.txt                   # Cấu hình biên dịch chuẩn CMake
└── README.md                        # Tài liệu hướng dẫn dự án
```

---

## 2. Các Mẫu Thiết Kế Hướng Đối Tượng (Design Patterns)

1. **Abstract Factory Pattern (TV1)**:
   - Interface `CloudResourceFactory` tạo ra một họ các tài nguyên tương thích (`ComputeResource`, `StorageResource`, `DatabaseResource`) cho từng nhà cung cấp riêng biệt mà không làm lộ các class cụ thể.
2. **Adapter Pattern (TV2)**:
   - Interface `IPricingProviderAdapter` chuẩn hóa các mô hình giá khác nhau của các hãng về cùng một đơn vị thống nhất (USD/tháng).
3. **Strategy Pattern (TV4)**:
   - Interface `ICostOptimizationStrategy` cho phép thay đổi, mở rộng linh hoạt các thuật toán tối ưu chi phí độc lập với mã nguồn gọi (`CheapestProviderStrategy`, `RightSizingStrategy`, `PricingModelStrategy`).
4. **Service Coordinator / Facade Pattern (TV3)**:
   - `MultiCloudOptimizationService` hiện thực hóa `ICostService`, đóng gói toàn bộ tương tác giữa `CostCalculator` (TV3), `CloudResourceFactory` (TV1), `PricingAdapter` (TV2) và `OptimizationStrategy` (TV4), giúp UI (TV5) chỉ cần giao tiếp qua interface sạch sẽ.

---

## 3. Hướng Dẫn Biên Dịch & Chạy

### Cách 1: Sử dụng build.bat (Đơn giản nhất trên Windows)
Mở terminal tại thư mục `Du an to` và gõ:
```cmd
.\build.bat
.\multi_cloud_optimizer.exe
```

### Cách 2: Sử dụng lệnh g++ trực tiếp
```bash
g++ -std=c++17 -Wall -Wextra main.cpp tv2/PricingAdapters.cpp tv3/CostCalculator.cpp tv3/SimulationEngine.cpp tv3/MultiCloudOptimizationService.cpp tv3/TestSuite.cpp tv4/CheapestProviderStrategy.cpp tv4/RightSizingStrategy.cpp tv4/PricingModelStrategy.cpp tv5/ConsoleUI.cpp -o multi_cloud_optimizer.exe
.\multi_cloud_optimizer.exe
```

### Cách 3: Chạy file demo riêng của từng thành viên
Mỗi thành viên có file demo độc lập trong thư mục của mình:
- **TV1**: `g++ -std=c++17 tv1/main_tv1.cpp -o demo_tv1.exe && .\demo_tv1.exe`
- **TV2**: `g++ -std=c++17 tv2/main_tv2.cpp tv2/PricingAdapters.cpp -o demo_tv2.exe && .\demo_tv2.exe`
- **TV3**: `g++ -std=c++17 tv3/main_tv3.cpp tv2/PricingAdapters.cpp tv3/CostCalculator.cpp tv3/SimulationEngine.cpp tv3/TestSuite.cpp -o demo_tv3.exe && .\demo_tv3.exe`
- **TV4**: `g++ -std=c++17 tv4/main_tv4.cpp tv2/PricingAdapters.cpp tv3/CostCalculator.cpp tv3/SimulationEngine.cpp tv4/CheapestProviderStrategy.cpp tv4/RightSizingStrategy.cpp tv4/PricingModelStrategy.cpp -o demo_tv4.exe && .\demo_tv4.exe`
- **TV5**: `g++ -std=c++17 tv5/main_tv5.cpp tv2/PricingAdapters.cpp tv3/CostCalculator.cpp tv3/SimulationEngine.cpp tv3/MultiCloudOptimizationService.cpp tv3/TestSuite.cpp tv4/CheapestProviderStrategy.cpp tv4/RightSizingStrategy.cpp tv4/PricingModelStrategy.cpp tv5/ConsoleUI.cpp -o demo_tv5.exe && .\demo_tv5.exe`

---

## 4. Bộ Kiểm Thử (Unit Tests)
Hệ thống tích hợp sẵn bộ kiểm thử tự động của TV3 với **25/25 test cases** bao phủ toàn bộ các trường hợp:
- Tính chi phí đơn lẻ và đa máy chủ
- Quy đổi thời gian (giờ/ngày/tháng/năm)
- Mô phỏng chi tiết từng dòng tài nguyên
- Tìm nhà cung cấp rẻ nhất & tính mức chênh lệch
- Xử lý hạ tầng rỗng
