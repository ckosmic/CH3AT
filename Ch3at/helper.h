#ifndef HELPER_H
#define HELPER_H

#include <cstdio>
#include <Windows.h>
#include <tlhelp32.h>
#include <iostream>
#include <string>
#include <SDL.h>
#include <SDL_syswm.h>
#include <SDL_ttf.h>
#include <vector>

using namespace std;

struct handleData {
	unsigned long pid;
	HWND windowHandle;
};

extern uint64_t baseAddress;

bool readFloatBigEndian(HANDLE hProcess, uint64_t location, float* out);
bool readByte(HANDLE hProcess, uint64_t location, int* out);
bool read4ByteBigEndian(HANDLE hProcess, uint64_t location, int* out);
bool read2ByteBigEndian(HANDLE hProcess, uint64_t location, int* out);
bool readString(HANDLE hProcess, uint64_t location, char* out, int length);
void writeFloatBigEndian(HANDLE hProcess, uint64_t location, float in);
void writeByte(HANDLE hProcess, uint64_t location, byte in);
void write4ByteBigEndian(HANDLE hProcess, uint64_t location, int in);
void write2ByteBigEndian(HANDLE hProcess, uint64_t location, int in);
void writeString(HANDLE hProcess, uint64_t location, const char* in, int length);
uint64_t getBaseAddress(HANDLE hProcess);

#endif // !HELPER_H

