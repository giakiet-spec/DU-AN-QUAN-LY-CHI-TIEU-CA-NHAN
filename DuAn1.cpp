#include <iostream>
#include <unordered_map>
#include <vector>
#include <mutex>
#include <shared_mutex>
#include <memory>
#include <string>
#include <thread>
#include <chrono>
#include <iomanip>

using namespace std;

struct A {
    int i;
    string u, p, n;
    double b;
    mutex m;
    A(int a, string b, string c, string d, double e) : i(a), u(b), p(c), n(d), b(e) {}
};

struct B {
    unordered_map<int, shared_ptr<A>> m;
    unordered_map<string, int> um;
    shared_mutex s;
    mutex k;
    unordered_map<int, vector<int>> g;
    unordered_map<int, int> v;

    void oa(int i, string u, string p, string n, double b) {
        unique_lock<shared_mutex> ul(s);
        if (m.find(i) == m.end()) {
            m[i] = make_shared<A>(i, u, p, n, b);
            um[u] = i;
        }
    }

    shared_ptr<A> lg(string u, string p) {
        shared_ptr<A> a;
        {
            shared_lock<shared_mutex> hl(s);
            if (um.find(u) == um.end()) return nullptr;
            a = m[um[u]];
        }
        lock_guard<mutex> l(a->m);
        if (a->p == p) return a;
        return nullptr;
    }

    bool tr(int u, int x, double a) {
        if (u == x || a <= 0) return false;
        shared_ptr<A> p1, p2;
        {
            shared_lock<shared_mutex> hl(s);
            if (m.find(u) == m.end() || m.find(x) == m.end()) return false;
            p1 = m[u];
            p2 = m[x];
        }
        scoped_lock lk(p1->m, p2->m);
        if (p1->b < a) return false;
        p1->b -= a;
        p2->b += a;
        {
            lock_guard<mutex> l(k);
            g[u].push_back(x);
        }
        return true;
    }

    bool df(int u) {
        v[u] = 1;
        for (int x : g[u]) {
            if (v[x] == 1) return true;
            if (v[x] == 0 && df(x)) return true;
        }
        v[u] = 2;
        return false;
    }

    bool dc() {
        lock_guard<mutex> l(k);
        v.clear();
        for (auto const& p : g) {
            if (v[p.first] == 0) {
                if (df(p.first)) return true;
            }
        }
        return false;
    }
};

void lb(string m) {
    int mx = 20;
    cout << m << "\n";
    for (int i = 0; i <= 100; i += 10) {
        cout << i << "% : |";
        int ps = mx * i / 100;
        for (int j = 0; j < mx; ++j) {
            if (j < ps) cout << "/";
            else cout << ".";
        }
        cout << "|\r";
        cout.flush();
        this_thread::sleep_for(chrono::milliseconds(40));
    }
    cout << "\n";
}

int main() {
    B bk;
    bk.oa(1, "account1", "12345678", "Minh", 5000000);
    bk.oa(2, "admin", "12345678", "Nguyen Duy Gia Kiet", 10000000);

    while (true) {
        string u, p;
        cout << "\n========== DANG NHAP HE THONG ==========\n";
        cout << "Username: "; 
        cin >> u;
        cout << "Password: "; 
        cin >> p;

        shared_ptr<A> a = bk.lg(u, p);
        if (a) {
            lb("Dang xac thuc tai khoan...");
            cout << "\nChao mung " << a->n << " tro lai!\n";
            lb("Dang tai du lieu cua " + a->n);
            cout << "=> Tai du lieu thanh cong!\n";

            int c;
            bool lo = false;
            do {
                cout << "\n--- HE THONG BANKING (" << a->n << ") ---\n";
                cout << "1. Chuyen tien\n";
                cout << "2. Xem so du\n";
                cout << "3. Kiem tra Cycle (Rua tien)\n";
                cout << "4. Dang xuat\n";
                cout << "5. Luu va Thoat\n";
                cout << "Lua chon: ";
                if (!(cin >> c)) {
                    cin.clear();
                    cin.ignore(1000, '\n');
                    continue;
                }

                switch (c) {
                    case 1: {
                        int x;
                        double v;
                        cout << "\n--- Nhap thong tin giao dich ---\n";
                        cout << "ID Nguoi nhan: "; cin >> x;
                        cout << "So tien: "; 
                        while(!(cin >> v)) {
                            cout << "Loi! Nhap lai so tien: ";
                            cin.clear();
                            cin.ignore(1000, '\n');
                        }
                        lb("Dang xu ly giao dich...");
                        if (bk.tr(a->i, x, v)) cout << "=> Giao dich thanh cong!\n";
                        else cout << "=> Giao dich that bai (Sai ID hoac thieu tien)!\n";
                        break;
                    }
                    case 2: {
                        lock_guard<mutex> l(a->m);
                        cout << "\nSO DU HIEN TAI CUA: " << a->n << "\n";
                        cout << "ID: " << a->i << " | So tien: " << fixed << setprecision(0) << a->b << " VND\n";
                        break;
                    }
                    case 3: {
                        lb("Dang quet he thong...");
                        if (bk.dc()) cout << "=> CANH BAO: Phat hien vong lap dong tien!\n";
                        else cout << "=> AN TOAN: Khong co dau hieu bat thuong.\n";
                        break;
                    }
                    case 4: {
                        lb("Dang luu du lieu an toan...");
                        lo = true;
                        cout << "=> " << a->n << " da dang xuat.\n";
                        break;
                    }
                    case 5: {
                        lb("Dang luu du lieu an toan...");
                        cout << "Tam biet!\n";
                        return 0;
                    }
                    default:
                        cout << "Lua chon khong hop le!\n";
                }
            } while (!lo);
        } else {
            cout << "Sai thong tin! Nhap '0' de thoat hoan toan hoac phim bat ky de thu lai: ";
            string op; cin >> op;
            if (op == "0") break;
        }
    }
    return 0;
}