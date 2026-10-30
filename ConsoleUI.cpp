// =====================================================================
// [TV5] ConsoleUI.cpp - Giao diện Console 
// =====================================================================
#include "ConsoleUI.h"
#include "../tv3/SimulationEngine.h"
#include "../tv3/TestSuite.h"
#include <iostream>   // cin, cout, setf, precision, left, right
#include <iomanip>    // setw, setfill
#include <fstream>    // ofstream
#include <string>     //  string, getline(cin, string), length, [], +=, string(n, c)
#include <vector>     //  vector, push_back, size (header của nhóm đã dùng)
#include <ctime>      //  time, localtime, tm

using namespace std;

// =========================
// 0. LỚP NGOẠI LỆ 
//
//                 LoiNhap             <- lớp cơ sở: giữ thông báo lỗi
//                    |
//          +---------+---------+
//          |                   |
//      KhongHopLe          HetDuLieu
//    (nhập sai -> nhập lại)   (hết dữ liệu -> thoát)
// =========================

class LoiNhap {
protected:
    string thongBao;
public:
    LoiNhap(string tb) { thongBao = tb; }
    string layThongBao() const { return thongBao; }
};

class KhongHopLe : public LoiNhap {
public:
    KhongHopLe(string tb) : LoiNhap(tb) {}
};

class HetDuLieu : public LoiNhap {
public:
    HetDuLieu() : LoiNhap("Het du lieu nhap") {}
};

// =========================
// 1. HÀM PHỤ
// =========================

bool laKhoangTrang(char c) {
    return c == ' ' || c == '\t' || c == '\r';
}

// Bỏ khoảng trắng ở 2 đầu xâu. Vd: "  bao cao.csv  " -> "bao cao.csv"
string catKhoangTrang(string s) {
    int dau  = 0;
    int cuoi = (int)s.length() - 1;                        // length(): độ dài xâu, thư viện <string>
    while (dau <= cuoi && laKhoangTrang(s[dau]))  dau++;   // s[i]: ký tự thứ i của xâu, thư viện <string>
    while (cuoi >= dau && laKhoangTrang(s[cuoi])) cuoi--;

    string kq = "";
    for (int i = dau; i <= cuoi; i++) kq += s[i];          //  +=: nối thêm 1 ký tự vào cuối xâu, thư viện <string>
    return kq;
}

// Mọi lần đọc bàn phím đều qua đây -> xử lý "hết dữ liệu" ở đúng 1 chỗ
string docDong(string loiNhac) {
    cout << loiNhac;
    string dong;
    //  getline(cin, xâu) - thư viện <string>: đọc cả dòng (kể cả dấu cách) vào string.
    // Đặt trong if(!...): đọc thất bại (Ctrl+Z / hết file) thì điều kiện đúng
    if (!getline(cin, dong)) throw HetDuLieu();
    return catKhoangTrang(dong);
}

// Nhập 1 số nguyên trong [minV, maxV]; sai thì báo lỗi và hỏi lại
int nhapSo(string loiNhac, int minV, int maxV) {
    while (true) {
        string dong = docDong(loiNhac);                    // HetDuLieu bay thẳng lên run()

        try {
            if (dong == "") throw KhongHopLe("Chua nhap gi");

            int so = 0;
            for (int i = 0; i < (int)dong.length(); i++) {
                if (dong[i] < '0' || dong[i] > '9') throw KhongHopLe("Chi duoc nhap chu so");
                so = so * 10 + (dong[i] - '0');            // '7' - '0' = 7
                // Vượt maxV là dừng ngay -> không bao giờ tràn int, đúng với mọi maxV
                if (so > maxV) throw KhongHopLe("So qua lon");
            }
            if (so < minV) throw KhongHopLe("So qua nho");
            return so;
        }
        catch (const KhongHopLe& e) {                      // chỉ bắt lỗi nhập sai; HetDuLieu đi qua
            cout << "Loi: " << e.layThongBao()
                 << ". Nhap lai (" << minV << " - " << maxV << ").\n";
        }
    }
}

// In mô tả 1 tài nguyên ra cout hoặc file (ostream là lớp cha của ofstream - Ch.3)
void inMoTa(ostream& out, const ResourceInput& r) {
    if (r.type == "COMPUTE")
        out << r.quantity << " may x " << r.vcpu << " vCPU / " << r.ramGB
            << "GB RAM (su dung " << r.usagePercent << "%)";
    else if (r.type == "STORAGE")
        out << r.sizeGB << "GB";
    else
        out << r.engine;
}

// In ngày giờ hiện tại dạng dd/mm/yyyy hh:mm
void inNgayGio(ostream& out) {
    time_t bayGio = time(0);                               //  time(0) - thư viện <ctime>: số giây từ 1/1/1970 đến giờ
    tm* t = localtime(&bayGio);                            //  localtime - thư viện <ctime>: tách thành ngày/tháng/năm/giờ/phút (struct tm)
    out << setfill('0')                                    // ô trống của setw điền '0' -> 05, 09
        << setw(2) << t->tm_mday << "/"
        << setw(2) << t->tm_mon + 1 << "/"                 // tm_mon đếm từ 0
        << t->tm_year + 1900 << " "                        // tm_year đếm từ 1900
        << setw(2) << t->tm_hour << ":"
        << setw(2) << t->tm_min
        << setfill(' ');                                   // trả lại dấu cách, nếu không các bảng sau bị điền '0'
}

bool chuaCoTaiNguyen(const vector<ResourceInput>& infra) {
    if (infra.size() == 0) {                               //  size(): số phần tử của vector, thư viện <vector>
        cout << "Chua co tai nguyen nao. Hay them tai nguyen truoc.\n";
        return true;
    }
    return false;
}

// =========================
// 2. HÀM TẠO & MENU
// =========================

ConsoleUI::ConsoleUI(ICostService& s, MultiCloudOptimizationService& fs)
    : service(s), fullService(fs) {}

void ConsoleUI::showMenu() const {
    cout << "\n===== HE THONG MO PHONG CHI PHI DA DAM MAY (AWS / GCP / Azure) =====\n"
         << "  1. Them may chu (Compute)\n"
         << "  2. Them luu tru (Storage)\n"
         << "  3. Them database\n"
         << "  4. Xem ha tang da thiet ke\n"
         << "  5. Tinh & so sanh chi phi\n"
         << "  6. Xem chi tiet tung nha cung cap\n"
         << "  7. Xem khuyen nghi toi uu\n"
         << "  8. Xuat bao cao CSV\n"
         << "  9. Chay bo kiem thu (TV3)\n"
         << "  0. Thoat\n";
}

// Vòng lặp chính. try bọc ngoài vòng lặp: HetDuLieu từ bất kỳ đâu đều nổi lên đây -> thoát êm
void ConsoleUI::run() {
    try {
        int chon;
        do {
            showMenu();
            chon = nhapSo("Chon chuc nang: ", 0, 9);
            switch (chon) {
                case 1: addCompute();          break;
                case 2: addStorage();          break;
                case 3: addDatabase();         break;
                case 4: showInfra();           break;
                case 5: showCosts();           break;
                case 6: showDetailedCosts();   break;
                case 7: showRecommendations(); break;
                case 8: exportCsv();           break;
                case 9: runTests();            break;
            }
        } while (chon != 0);
    }
    catch (const HetDuLieu& e) {
        cout << "\n" << e.layThongBao() << ", ket thuc chuong trinh.\n";
        return;
    }
    cout << "\nCam on da su dung he thong!\n";
}

// =========================
// 3. THÊM TÀI NGUYÊN
// =========================

void ConsoleUI::addCompute() {
    ResourceInput r;
    r.type         = "COMPUTE";
    r.quantity     = nhapSo("So luong may chu: ", 1, 100);
    r.vcpu         = nhapSo("So vCPU moi may: ", 1, 128);
    r.ramGB        = nhapSo("RAM moi may (GB): ", 1, 1024);
    r.usagePercent = nhapSo("Muc su dung gia dinh (%): ", 1, 100);   // TV4 RightSizing cần
    infra.push_back(r);                                    //  push_back(): thêm phần tử vào cuối vector, thư viện <vector>
    cout << "-> Da them may chu: ";
    inMoTa(cout, r);
    cout << "\n";
}

void ConsoleUI::addStorage() {
    ResourceInput r;
    r.type   = "STORAGE";
    r.sizeGB = nhapSo("Dung luong (GB): ", 1, 100000);
    infra.push_back(r);
    cout << "-> Da them luu tru: ";
    inMoTa(cout, r);
    cout << "\n";
}

void ConsoleUI::addDatabase() {
    ResourceInput r;
    r.type   = "DATABASE";
    r.engine = (nhapSo("Engine (1 = PostgreSQL, 2 = MySQL): ", 1, 2) == 1) ? "PostgreSQL" : "MySQL";
    infra.push_back(r);
    cout << "-> Da them database: ";
    inMoTa(cout, r);
    cout << "\n";
}

// =========================
// 4. XEM HẠ TẦNG
// =========================

void ConsoleUI::showInfra() const {
    if (chuaCoTaiNguyen(infra)) return;

    cout << "\n===== HA TANG DA THIET KE =====\n";
    for (int i = 0; i < (int)infra.size(); i++) {
        cout << i + 1 << ". " << infra[i].type << ": ";
        inMoTa(cout, infra[i]);
        cout << "\n";
    }
}

// =========================
// 5. SO SÁNH CHI PHÍ
// =========================

void ConsoleUI::showCosts() const {
    if (chuaCoTaiNguyen(infra)) return;

    vector<ProviderCost> costs = service.calculate(infra);
    int n = (int)costs.size();

    // Tìm tổng lớn nhất (để vẽ biểu đồ) và vị trí hãng rẻ nhất
    double maxTotal = 0;
    int reNhat = 0;
    for (int i = 0; i < n; i++) {
        if (costs[i].total() > maxTotal)              maxTotal = costs[i].total();
        if (costs[i].total() < costs[reNhat].total()) reNhat = i;
    }

    cout.setf(ios::fixed);                                 // số thực dạng thập phân cố định
    cout.precision(2);                                     // 2 chữ số sau dấu phẩy

    cout << "\n==================== CHI PHI UOC TINH (USD) ====================\n";
    cout << left << setw(8) << "Hang" << right
         << setw(11) << "May chu" << setw(11) << "Luu tru" << setw(11) << "Database"
         << setw(12) << "TONG/thang" << setw(12) << "TONG/nam" << "\n";
    for (int i = 0; i < n; i++) {
        cout << left << setw(8) << costs[i].provider << right
             << setw(11) << costs[i].compute
             << setw(11) << costs[i].storage
             << setw(11) << costs[i].database
             << setw(12) << costs[i].total()
             << setw(12) << costs[i].total() * MONTHS_PER_YEAR << "\n";
    }

    // Biểu đồ cột: hãng đắt nhất = 40 dấu '#'
    cout << "\nBieu do so sanh:\n";
    for (int i = 0; i < n; i++) {
        int soCot = (maxTotal > 0) ? (int)(costs[i].total() / maxTotal * 40) : 0;
        cout << left << setw(8) << costs[i].provider
             << string(soCot, '#')                         //  string(n, c): xâu gồm n ký tự c, thư viện <string>
             << " $" << costs[i].total() << "\n";
    }

    cout << "\n>> Re nhat: " << costs[reNhat].provider
         << " ($" << costs[reNhat].total() << "/thang)\n";
}

// =========================
// 6. CHI TIẾT TỪNG HÃNG (bộ mô phỏng của TV3)
// =========================

void ConsoleUI::showDetailedCosts() const {
    if (chuaCoTaiNguyen(infra)) return;

    SimulationEngine engine(fullService.getCalculator());
    vector<string> hang = fullService.getCalculator().getProviderNames();
    for (int i = 0; i < (int)hang.size(); i++)
        engine.printDetail(hang[i], infra);

    engine.printComparison(infra, TimeUnit::Hour);
    engine.printComparison(infra, TimeUnit::Month);
    engine.printComparison(infra, TimeUnit::Year);
}

// =========================
// 7. KHUYẾN NGHỊ TỐI ƯU
// =========================

void ConsoleUI::showRecommendations() const {
    if (chuaCoTaiNguyen(infra)) return;

    vector<Recommendation> recs = service.recommend(infra);
    if (recs.size() == 0) {
        cout << "Khong co khuyen nghi nao.\n";
        return;
    }

    cout.setf(ios::fixed);
    cout.precision(2);
    cout << "\n========== KHUYEN NGHI TOI UU ==========\n";
    for (int i = 0; i < (int)recs.size(); i++) {
        cout << i + 1 << ". [" << recs[i].strategy << "] " << recs[i].description << "\n"
             << "   Truoc: $" << recs[i].before << "  |  Sau: $" << recs[i].after
             << "  >> Tiet kiem: $" << recs[i].saving() << " (" << recs[i].percent() << "%)\n";
    }
    cout << "Luu y: cac khuyen nghi tinh doc lap, KHONG cong don tien tiet kiem.\n";
}

// =========================
// 8. XUẤT CSV
// =========================

void ConsoleUI::exportCsv() const {
    if (chuaCoTaiNguyen(infra)) return;

    string ten = docDong("Ten file (Enter = baocao.csv): ");
    if (ten == "") ten = "baocao.csv";

    ofstream f;
    f.open(ten);
    if (!f) {
        cout << "Khong tao duoc file.\n";
        return;
    }

    f.setf(ios::fixed);                                    // định dạng số riêng cho file
    f.precision(2);
    f << "BAO CAO MO PHONG CHI PHI HA TANG DA DAM MAY\n";
    f << "Ngay xuat,";
    inNgayGio(f);
    f << "\n";
    f << "Vung mac dinh," << DEFAULT_REGION << "\n";
    f << "Luu y,Gia mo phong - khong phai bao gia cam ket\n\n";

    // --- Phần 1: hạ tầng ---
    f << "HA TANG DA THIET KE\nSTT,Loai,Chi tiet\n";
    for (int i = 0; i < (int)infra.size(); i++) {
        f << i + 1 << "," << infra[i].type << ",";
        inMoTa(f, infra[i]);
        f << "\n";
    }

    // --- Phần 2: so sánh chi phí ---
    f << "\nSO SANH CHI PHI (USD)\n";
    f << "Hang,May chu/thang,Luu tru/thang,Database/thang,Tong/thang,Tong/nam\n";
    vector<ProviderCost> costs = service.calculate(infra);
    for (int i = 0; i < (int)costs.size(); i++) {
        f << costs[i].provider << "," << costs[i].compute << "," << costs[i].storage << ","
          << costs[i].database << "," << costs[i].total() << ","
          << costs[i].total() * MONTHS_PER_YEAR << "\n";
    }

    // --- Phần 3: khuyến nghị (mô tả trong ngoặc kép vì có thể chứa dấu phẩy) ---
    f << "\nKHUYEN NGHI TOI UU\n";
    f << "Chien luoc,Mo ta,Truoc (USD),Sau (USD),Tiet kiem (USD),Tiet kiem (%)\n";
    vector<Recommendation> recs = service.recommend(infra);
    for (int i = 0; i < (int)recs.size(); i++) {
        f << recs[i].strategy << ",\"" << recs[i].description << "\"," << recs[i].before << ","
          << recs[i].after << "," << recs[i].saving() << "," << recs[i].percent() << "\n";
    }

    f.close();
    cout << ">> Da xuat bao cao: " << ten << "\n";
}

// =========================
// 9. BỘ KIỂM THỬ CỦA TV3
// =========================

void ConsoleUI::runTests() const {
    TestSuite tests;
    tests.runAll();
}
