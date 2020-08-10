#include "table.h"

using namespace std;

int main(int argc, char* argv[]) {
    ShowWindow(GetConsoleWindow(), SW_HIDE);
    openTable();

    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

    if (Process32First(snapshot, &entry) == TRUE) {
        while (Process32Next(snapshot, &entry) == TRUE) {
            if (wcscmp(entry.szExeFile, L"rpcs3.exe") == 0) {
                HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, entry.th32ProcessID);

                loadValues(hProcess);
                sdlWindow(entry.th32ProcessID, hProcess);
               
                CloseHandle(hProcess);
            }
        }
    }

    CloseHandle(snapshot);

	return 1;
}