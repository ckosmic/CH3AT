#ifndef TABLE_H
#define TABLE_H

#include <Windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdio>

#include "rapidxml/rapidxml.hpp"
#include "helper.h"

#define VERSION "1.0.0"

using namespace std;
using namespace rapidxml;

enum VariableTypes {
	Binary,
	Byte,
	Bytes2,
	Bytes4,
	Bytes8,
	Float,
	Double,
	String,
	ArrayOfByte,
	All,
	Grouped,
	Byte2BigEndian,
	Byte4BigEndian,
	FloatBigEndian,
	Custom
};

enum ValueType {
	VT_Int,
	VT_Float,
	VT_String
};

struct TextBoxCollider {
	int x, y, w, h;
	bool pressed;
	bool mouseover;
};

struct button_t {
	uint32_t x;
	uint32_t y;
	char* text;
	TTF_Font* font;
	SDL_Color textColor;
	struct {
		uint8_t r, g, b, a;
	} color;
	bool pressed;
	bool mouseover;
};

class CheatEntry {
private:
	
public:
	string description;
	VariableTypes variableType;
	uint64_t address;
	vector<CheatEntry> cheatEntries;
	
	int intValue;
	float floatValue;
	char stringValue[256];

	int default_intValue;
	float default_floatValue;
	char default_stringValue[256];

	int stringLength;
	ValueType valueType;

	bool rerender = true;
};

extern vector<CheatEntry> cheatEntries;

void openTable();
void loadValues(HANDLE hProcess);
HWND findMainWindow(unsigned long pid);
BOOL CALLBACK enumWindowsCallback(HWND handle, LPARAM lParam);
bool isMainWindow(HWND handle);
void sdlWindow(unsigned long pid, HANDLE hProcess);

#endif // !TABLE_H
