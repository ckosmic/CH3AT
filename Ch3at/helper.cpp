#include "helper.h"

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

bool readByte(HANDLE hProcess, uint64_t location, int* out) {
    LPCVOID lpMem = (void*)(location);
    byte buffer;
    if (ReadProcessMemory(hProcess, lpMem, (LPVOID)(&buffer), 1, NULL)) {
        *out = (int)buffer;
        return true;
    }
    return false;
}

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

bool readString(HANDLE hProcess, uint64_t location, char* out, int length) {
    LPCVOID lpMem = (void*)(location);
    byte buffer[256];
    if (ReadProcessMemory(hProcess, lpMem, (LPVOID)(&buffer), length, NULL)) {
        memcpy(out, buffer, 256);
        return true;
    }
    return false;
}

void writeFloatBigEndian(HANDLE hProcess, uint64_t location, float in) {
    LPVOID lpMem = (void*)(location);
    byte* buffer = reinterpret_cast<byte*>(&in);
    reverse(buffer, buffer + 4);
    WriteProcessMemory(hProcess, lpMem, (LPCVOID)(buffer), 4, NULL);
}

void writeByte(HANDLE hProcess, uint64_t location, byte in) {
    LPVOID lpMem = (void*)(location);
    WriteProcessMemory(hProcess, lpMem, (LPCVOID)(&in), 1, NULL);
}

void write4ByteBigEndian(HANDLE hProcess, uint64_t location, int in) {
    LPVOID lpMem = (void*)(location);
    byte* buffer = reinterpret_cast<byte*>(&in);
    reverse(buffer, buffer + 4);
    WriteProcessMemory(hProcess, lpMem, (LPCVOID)(buffer), 4, NULL);
}

void write2ByteBigEndian(HANDLE hProcess, uint64_t location, int in) {
    LPVOID lpMem = (void*)(location);
    byte* buffer = reinterpret_cast<byte*>(&in);
    reverse(buffer, buffer + 2);
    WriteProcessMemory(hProcess, lpMem, (LPCVOID)(buffer), 2, NULL);
}

void writeString(HANDLE hProcess, uint64_t location, const char* in, int length) {
    LPVOID lpMem = (void*)(location);
    WriteProcessMemory(hProcess, lpMem, (LPCVOID)(in), 4, NULL);
}

