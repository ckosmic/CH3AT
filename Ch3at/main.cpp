#include "table.h"

using namespace std;

int main(int argc, char* argv[]) {
	ShowWindow(GetConsoleWindow(), SW_HIDE);

	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	bool foundRpcs3 = false;
	if (Process32First(snapshot, &entry) == TRUE) {
		while (Process32Next(snapshot, &entry) == TRUE) {
			if (wcscmp(entry.szExeFile, L"rpcs3.exe") == 0) {
				foundRpcs3 = true;

				HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, entry.th32ProcessID);

				baseAddress = getBaseAddress(hProcess);
				cout << hex << baseAddress << endl;

				openTable();
				openFrozenTable();

				loadValues(hProcess);
				loadValuesFrozen(hProcess);
				sdlWindow(entry.th32ProcessID, hProcess);
			   
				CloseHandle(hProcess);
			}
		}
	}

	if (!foundRpcs3) {
		int choice = MessageBoxA(NULL, "RPCS3 is not currently running.", "CH3AT Error", MB_ICONERROR);
	}

	CloseHandle(snapshot);
	exit(0);

	return 1;
}