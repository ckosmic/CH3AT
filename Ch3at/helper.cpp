#include "helper.h"

uint64_t baseAddress;

// Read a float from memory
bool readFloatBigEndian(HANDLE hProcess, uint64_t location, float* out) {
	LPCVOID lpMem = (void*)(location);
	byte buffer[4];
	if (ReadProcessMemory(hProcess, lpMem, (LPVOID)(&buffer), 4, NULL)) {
		reverse(buffer, buffer + 4);
		memcpy(out, buffer, sizeof(float));
		return true;
	}
	return false;
}

// Read a byte from memory
bool readByte(HANDLE hProcess, uint64_t location, int* out) {
	LPCVOID lpMem = (void*)(location);
	byte buffer;
	if (ReadProcessMemory(hProcess, lpMem, (LPVOID)(&buffer), 1, NULL)) {
		*out = (int)buffer;
		return true;
	}
	return false;
}

// Read a big endian 4 byte number from memory
bool read4ByteBigEndian(HANDLE hProcess, uint64_t location, int* out) {
	LPCVOID lpMem = (void*)(location);
	byte buffer[4];
	if (ReadProcessMemory(hProcess, lpMem, (LPVOID)(&buffer), 4, NULL)) {
		reverse(buffer, buffer + 4);
		memcpy(out, buffer, sizeof(int));
		return true;
	}
	return false;
}

// Read a big endian 2 byte number from memory
bool read2ByteBigEndian(HANDLE hProcess, uint64_t location, int* out) {
	LPCVOID lpMem = (void*)(location);
	byte buffer[2];
	if (ReadProcessMemory(hProcess, lpMem, (LPVOID)(&buffer), 2, NULL)) {
		reverse(buffer, buffer + 2);
		memcpy(out, buffer, sizeof(int));
		return true;
	}
	return false;
}

// Read a string from memory
bool readString(HANDLE hProcess, uint64_t location, char* out, int length) {
	LPCVOID lpMem = (void*)(location);
	byte buffer[256];
	if (ReadProcessMemory(hProcess, lpMem, (LPVOID)(&buffer), length, NULL)) {
		memcpy(out, buffer, 256);
		return true;
	}
	return false;
}

// Write a float to memory
void writeFloatBigEndian(HANDLE hProcess, uint64_t location, float in) {
	LPVOID lpMem = (void*)(location);
	byte* buffer = reinterpret_cast<byte*>(&in);
	reverse(buffer, buffer + 4);
	WriteProcessMemory(hProcess, lpMem, (LPCVOID)(buffer), 4, NULL);
}

// Write a byte to memory
void writeByte(HANDLE hProcess, uint64_t location, byte in) {
	LPVOID lpMem = (void*)(location);
	WriteProcessMemory(hProcess, lpMem, (LPCVOID)(&in), 1, NULL);
}

// Write a big endian 4 byte number to memory
void write4ByteBigEndian(HANDLE hProcess, uint64_t location, int in) {
	LPVOID lpMem = (void*)(location);
	byte* buffer = reinterpret_cast<byte*>(&in);
	reverse(buffer, buffer + 4);
	WriteProcessMemory(hProcess, lpMem, (LPCVOID)(buffer), 4, NULL);
}

// Write a big endian 2 byte number to memory
void write2ByteBigEndian(HANDLE hProcess, uint64_t location, int in) {
	LPVOID lpMem = (void*)(location);
	byte* buffer = reinterpret_cast<byte*>(&in);
	reverse(buffer, buffer + 2);
	WriteProcessMemory(hProcess, lpMem, (LPCVOID)(buffer), 2, NULL);
}

// Write a string to memory
void writeString(HANDLE hProcess, uint64_t location, const char* in, int length) {
	LPVOID lpMem = (void*)(location);
	WriteProcessMemory(hProcess, lpMem, (LPCVOID)(in), 4, NULL);
}

// Gets a base address by looking for (hopefully) unchanging values
uint64_t getBaseAddress(HANDLE hProcess) {
	int i = 0;
	char a[] = { 0x30, 0x2F, 0x90, 0x20, 0x30, 0x9E, 0xCC, 0xD0, 0x40, 0xE5, 0xB9, 0x70, 0x00, 0x00, 0x00, 0x00 };
	for (i = 0; i < 134217728; i += 32768) {
		char buffer[32768];
		uint64_t location = 0x330000000 + i;
		LPVOID lpMem = (void*)location;
		SIZE_T* readNum = nullptr;
		if (ReadProcessMemory(hProcess, lpMem, (LPVOID)(&buffer), 32768, readNum)) {
			string needle(a, a + 16);
			string haystack(buffer, buffer + 32768);

			size_t n = haystack.find(needle);
			if (n != string::npos) {
				return (location + n);
			}
		}
	}
	return 0;
}