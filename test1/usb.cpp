#include <windows.h>
#include <iostream>
#include <vector>
#include <string>

using namespace std;

// Fungsi untuk mendeteksi port COM yang tersedia
vector<string> getAvailableCOMPorts() {
    vector<string> comPorts;
    for (int i = 1; i <= 256; i++) {  // Cek COM1 - COM256
        string portName = "\\\\.\\COM" + to_string(i);
        HANDLE hPort = CreateFileA(portName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
        if (hPort != INVALID_HANDLE_VALUE) {
            comPorts.push_back("COM" + to_string(i));
            CloseHandle(hPort);
        }
    }
    return comPorts;
}

int main() {
    // 1. Tampilkan daftar COM yang tersedia
    vector<string> ports = getAvailableCOMPorts();
    if (ports.empty()) {
        cout << "Tidak ada port COM yang tersedia!" << endl;
        return 1;
    }

    cout << "Port COM yang tersedia:\n";
    for (size_t i = 0; i < ports.size(); i++) {
        cout << i + 1 << ". " << ports[i] << endl;
    }

    // 2. Meminta input pengguna untuk memilih COM
    int pilihan;
    cout << "Pilih nomor COM: ";
    cin >> pilihan;

    if (pilihan < 1 || pilihan > ports.size()) {
        cout << "Pilihan tidak valid!" << endl;
        return 1;
    }

    string selectedCOM = "\\\\.\\";
    selectedCOM += ports[pilihan - 1];  // COM yang dipilih

    // 3. Membuka port COM
    HANDLE hSerial = CreateFileA(selectedCOM.c_str(), GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
    if (hSerial == INVALID_HANDLE_VALUE) {
        cout << "Gagal membuka " << ports[pilihan - 1] << "!" << endl;
        return 1;
    }

    // 4. Konfigurasi port
    DCB dcbSerialParams = {0};
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    GetCommState(hSerial, &dcbSerialParams);
    dcbSerialParams.BaudRate = CBR_9600;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.Parity = NOPARITY;
    dcbSerialParams.StopBits = ONESTOPBIT;
    SetCommState(hSerial, &dcbSerialParams);

    // 5. Membaca data dari port COM
    char buffer[256];
    DWORD bytesRead;
    cout << "Membaca data dari " << ports[pilihan - 1] << "...\n";
    while (true) {
        if (ReadFile(hSerial, buffer, sizeof(buffer) - 1, &bytesRead, NULL) && bytesRead > 0) {
            buffer[bytesRead] = '\0';  // Tambahkan null-terminator
            cout << "Data: " << buffer << endl;
        }
        Sleep(1000);
    }

    CloseHandle(hSerial);
    return 0;
}
