#include "table.h"

vector<CheatEntry> cheatEntries;
vector<CheatEntry> freezeEntries;

bool windowShown = true;
bool quit = false;
float freezeUpdate = 1000.0/60.0;

// Opens and parses the cheat table, places it into the cheatEntries vector
void openTable() {
	ifstream file("Skate 3.CT");
	string contents((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
	file.close();

	xml_document<> doc;
	doc.parse<0>((char*)contents.c_str());

	xml_node<>* node = doc.first_node("CheatTable")->first_node("CheatEntries");
	for (xml_node<>* entryNode = node->first_node("CheatEntry"); entryNode; entryNode = entryNode->next_sibling()) {
		CheatEntry ce = CheatEntry();
		xml_node<>* addr = entryNode->first_node("Address");
		xml_node<>* offs = entryNode->first_node("Offset");
		uint64_t address = 0;
		if (addr != nullptr) {
			address = strtoull(addr->value(), NULL, 16);
		}
		if (offs != nullptr) {
			address = strtoull(offs->value(), NULL, 16) + baseAddress;
		}

		ce.address = address;

		ce.description = entryNode->first_node("Description")->value();

		string type = entryNode->first_node("VariableType")->value();
		xml_node<>* customTypeNode = entryNode->first_node("CustomType");
		string customType = "null";
		if(customTypeNode != nullptr)
			customType = customTypeNode->value();
		if (type == "Binary") {
			ce.variableType = Binary;
		} else if (type == "Byte") {
			ce.variableType = Byte;
		} else if (type == "2 Bytes") {
			ce.variableType = Bytes2;
		} else if (type == "4 Bytes") {
			ce.variableType = Bytes4;
		} else if (type == "8 Bytes") {
			ce.variableType = Bytes8;
		} else if (type == "Float") {
			ce.variableType = Float;
		} else if (type == "Double") {
			ce.variableType = Double;
		} else if (type == "String") {
			ce.variableType = String;
			ce.stringLength = atoi(entryNode->first_node("Length")->value());
		} else if (type == "Array of byte") {
			ce.variableType = ArrayOfByte;
		} else if (type == "All") {
			ce.variableType = All;
		} else if (type == "Grouped") {
			ce.variableType = Grouped;
		} else if (type == "Custom") {
			if (customType == "4 Byte Big Endian") {
				ce.variableType = Byte4BigEndian;
			} else if (customType == "2 Byte Big Endian") {
				ce.variableType = Byte2BigEndian;
			} else if (customType == "Float Big Endian") {
				ce.variableType = FloatBigEndian;
			}
		}

		cheatEntries.push_back(ce);
	}
}

// Load values from memory into the cheat entries
void loadValues(HANDLE hProcess) {
	for (int i = 0; i < cheatEntries.size(); i++) {
		switch (cheatEntries[i].variableType) {
		case Byte:
			readByte(hProcess, cheatEntries[i].address, &(cheatEntries[i].intValue));
			cheatEntries[i].default_intValue = cheatEntries[i].intValue;
			cheatEntries[i].valueType = VT_Int;
			break;
		case Byte2BigEndian:
			read2ByteBigEndian(hProcess, cheatEntries[i].address, &(cheatEntries[i].intValue));
			cheatEntries[i].default_intValue = cheatEntries[i].intValue;
			cheatEntries[i].valueType = VT_Int;
			break;
		case Byte4BigEndian:
			read4ByteBigEndian(hProcess, cheatEntries[i].address, &(cheatEntries[i].intValue));
			cheatEntries[i].default_intValue = cheatEntries[i].intValue;
			cheatEntries[i].valueType = VT_Int;
			break;
		case FloatBigEndian:
			readFloatBigEndian(hProcess, cheatEntries[i].address, &(cheatEntries[i].floatValue));
			cheatEntries[i].default_floatValue = cheatEntries[i].floatValue;
			cheatEntries[i].valueType = VT_Float;
			break;
		case String:
			readString(hProcess, cheatEntries[i].address, cheatEntries[i].stringValue, cheatEntries[i].stringLength);
			memcpy(cheatEntries[i].default_stringValue, cheatEntries[i].stringValue, 256);
			cheatEntries[i].valueType = VT_String;
			break;
		}
	}
}

void openFrozenTable() {
	ifstream file("frozen_entries.xml");
	string contents((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
	file.close();

	xml_document<> doc;
	doc.parse<0>((char*)contents.c_str());

	freezeUpdate = atof(doc.first_node("FrozenTable")->first_node("UpdateTime")->value());

	xml_node<>* node = doc.first_node("FrozenTable")->first_node("FrozenEntries");
	for (xml_node<>* entryNode = node->first_node("FrozenEntry"); entryNode; entryNode = entryNode->next_sibling()) {
		CheatEntry ce = CheatEntry();
		xml_node<>* addr = entryNode->first_node("Address");
		xml_node<>* offs = entryNode->first_node("Offset");
		uint64_t address;
		if (addr != nullptr) {
			address = strtoull(addr->value(), NULL, 16);
		}
		if (offs != nullptr) {
			address = strtoull(offs->value(), NULL, 16) + baseAddress;
		}
		
		ce.address = address;

		ce.description = entryNode->first_node("Description")->value();

		string type = entryNode->first_node("VariableType")->value();
		xml_node<>* customTypeNode = entryNode->first_node("CustomType");
		string customType = "null";
		if (customTypeNode != nullptr)
			customType = customTypeNode->value();
		if (type == "Binary") {
			ce.variableType = Binary;
		}
		else if (type == "Byte") {
			ce.variableType = Byte;
		}
		else if (type == "2 Bytes") {
			ce.variableType = Bytes2;
		}
		else if (type == "4 Bytes") {
			ce.variableType = Bytes4;
		}
		else if (type == "8 Bytes") {
			ce.variableType = Bytes8;
		}
		else if (type == "Float") {
			ce.variableType = Float;
		}
		else if (type == "Double") {
			ce.variableType = Double;
		}
		else if (type == "String") {
			ce.variableType = String;
		}
		else if (type == "Array of byte") {
			ce.variableType = ArrayOfByte;
		}
		else if (type == "All") {
			ce.variableType = All;
		}
		else if (type == "Grouped") {
			ce.variableType = Grouped;
		}
		else if (type == "Custom") {
			if (customType == "4 Byte Big Endian") {
				ce.variableType = Byte4BigEndian;
			}
			else if (customType == "2 Byte Big Endian") {
				ce.variableType = Byte2BigEndian;
			}
			else if (customType == "Float Big Endian") {
				ce.variableType = FloatBigEndian;
			}
		}

		freezeEntries.push_back(ce);
	}
}

// Load values from memory into the cheat entries and set their default values/value type
void loadValuesFrozen(HANDLE hProcess) {
	for (int i = 0; i < freezeEntries.size(); i++) {
		switch (freezeEntries[i].variableType) {
		case Byte:
			readByte(hProcess, freezeEntries[i].address, &(freezeEntries[i].intValue));
			freezeEntries[i].default_intValue = freezeEntries[i].intValue;
			freezeEntries[i].valueType = VT_Int;
			break;
		case Byte2BigEndian:
			read2ByteBigEndian(hProcess, freezeEntries[i].address, &(freezeEntries[i].intValue));
			freezeEntries[i].default_intValue = freezeEntries[i].intValue;
			freezeEntries[i].valueType = VT_Int;
			break;
		case Byte4BigEndian:
			read4ByteBigEndian(hProcess, freezeEntries[i].address, &(freezeEntries[i].intValue));
			freezeEntries[i].default_intValue = freezeEntries[i].intValue;
			freezeEntries[i].valueType = VT_Int;
			break;
		case FloatBigEndian:
			readFloatBigEndian(hProcess, freezeEntries[i].address, &(freezeEntries[i].floatValue));
			freezeEntries[i].default_floatValue = freezeEntries[i].floatValue;
			freezeEntries[i].valueType = VT_Float;
			break;
		case String:
			readString(hProcess, freezeEntries[i].address, freezeEntries[i].stringValue, freezeEntries[i].stringLength);
			memcpy(freezeEntries[i].default_stringValue, freezeEntries[i].stringValue, 256);
			freezeEntries[i].valueType = VT_String;
			break;
		}
	}
}

// Load a value from memory into a cheat entry
void loadValueFrozen(HANDLE hProcess, int i) {
	switch (freezeEntries[i].variableType) {
	case Byte:
		readByte(hProcess, freezeEntries[i].address, &(freezeEntries[i].intValue));
		break;
	case Byte2BigEndian:
		read2ByteBigEndian(hProcess, freezeEntries[i].address, &(freezeEntries[i].intValue));
		break;
	case Byte4BigEndian:
		read4ByteBigEndian(hProcess, freezeEntries[i].address, &(freezeEntries[i].intValue));
		break;
	case FloatBigEndian:
		readFloatBigEndian(hProcess, freezeEntries[i].address, &(freezeEntries[i].floatValue));
		break;
	case String:
		readString(hProcess, freezeEntries[i].address, freezeEntries[i].stringValue, freezeEntries[i].stringLength);
		break;
	}
}

HWND findMainWindow(unsigned long pid) {
	handleData data;
	data.pid = pid;
	data.windowHandle = 0;
	EnumWindows(enumWindowsCallback, (LPARAM)&data);
	return data.windowHandle;
}

BOOL CALLBACK enumWindowsCallback(HWND handle, LPARAM lParam) {
	handleData& data = *(handleData*)lParam;
	unsigned long pid = 0;
	GetWindowThreadProcessId(handle, &pid);
	if (data.pid != pid || !isMainWindow(handle))
		return true;
	data.windowHandle = handle;
	return false;
}

bool isMainWindow(HWND handle) {
	char windowTitle[256];
	GetWindowTextA(handle, windowTitle, 256);
	return (
		GetWindow(handle, GW_OWNER) == (HWND)0 &&
		IsWindowVisible(handle) &&
		strstr(windowTitle, "RPCS3") == nullptr &&
		isGameWindow((string)windowTitle)
		);
}

bool isGameWindow(string windowTitle) {
	const vector<string> validGameIds = { "BLES00760","BLJM60296","BLJM60437","BLJM61141","BLUS30464" };
	for (string const& gameId : validGameIds) {
		/* Check if the window title contains one of the valid game ID's for Skate 3*/
		if (windowTitle.find(gameId) != string::npos) {
			return true;
		}
	}
	return false;
}

bool loadFont(const char* fontName, int size, TTF_Font* font) {
	font = TTF_OpenFont(fontName, size);
	if (!font)
		return false;
	return true;
}

void printText(SDL_Renderer* renderer, TTF_Font* font, char* text, int x, int y, SDL_Color color) {
	SDL_Surface* text_surface;
	if (!(text_surface = TTF_RenderText_Solid(font, text, color))) {
		SDL_FreeSurface(text_surface);
	} else {
		SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
		SDL_FreeSurface(text_surface);
		SDL_Rect srcRect;
		srcRect.x = 0;
		srcRect.y = 0;
		SDL_QueryTexture(text_texture, NULL, NULL, &(srcRect.w), &(srcRect.h));
		SDL_Rect dstRect;
		dstRect.x = x;
		dstRect.y = y;
		dstRect.w = srcRect.w;
		dstRect.h = srcRect.h;
		SDL_RenderCopy(renderer, text_texture, &srcRect, &dstRect);
		SDL_DestroyTexture(text_texture);
	}
}

void printText(SDL_Renderer* renderer, TTF_Font* font, const char* text, int x, int y, SDL_Color color) {
	printText(renderer, font, (char*)text, x, y, color);
}

void drawTextbox(SDL_Renderer* renderer, TTF_Font* font, char* text, SDL_Rect rect, SDL_Color color) {
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	SDL_RenderFillRect(renderer, &rect);
	SDL_SetRenderDrawColor(renderer, color.r - 50, color.g - 50, color.b - 50, color.a);
	SDL_RenderDrawRect(renderer, &rect);
	printText(renderer, font, text, rect.x + 10, rect.y + 2, { 0, 0, 0 });
}

void drawTextbox(SDL_Renderer* renderer, TTF_Font* font, const char* text, SDL_Rect rect, SDL_Color color) {
	drawTextbox(renderer, font, (char*)text, rect, color);
}

int textBoxEvent(SDL_Rect rect, SDL_Event* e) {
	if (e->type == SDL_MOUSEBUTTONDOWN) {
		if (e->button.x >= rect.x && e->button.x <= rect.x + rect.w && e->button.y >= rect.y && e->button.y <= rect.y + rect.h) {
			if (e->button.button == SDL_BUTTON_LEFT)
				return 1;
			else if (e->button.button == SDL_BUTTON_MIDDLE)
				return 2;
		}
	}
	return 0;
}

// Tests if string is a number (supports negative numbers and decimals)
bool isDigits(string str) {
	size_t decCount = count(str.begin(), str.end(), '.');
	size_t negCount = count(str.begin(), str.end(), '-');
	return str.find_first_not_of("0123456789.-") == string::npos && decCount <= 1 && ((negCount == 1 && str[0] == '-') || negCount == 0) && str.size() > 0;
}

// Converts memory values to strings to display in the textboxes
string getStringValue(int index) {
	string value;
	switch (cheatEntries[index].valueType) {
	case VT_Int:
		value = to_string(cheatEntries[index].intValue);
		break;
	case VT_Float:
		value = to_string(cheatEntries[index].floatValue);
		int offset;
		offset = 1;
		if (value.find_last_not_of('0') == value.find('.')) offset = 0;
		value.erase(value.find_last_not_of('0') + offset, string::npos);
		break;
	case VT_String:
		value = cheatEntries[index].stringValue;
		break;
	}
	return value;
}

// Converts memory values of frozen addresses to strings to display in the textboxes
string getStringValueFrozen(int index) {
	index -= cheatEntries.size();
	string value;
	switch (freezeEntries[index].valueType) {
	case VT_Int:
		value = to_string(freezeEntries[index].intValue);
		break;
	case VT_Float:
		value = to_string(freezeEntries[index].floatValue);
		int offset;
		offset = 1;
		if (value.find_last_not_of('0') == value.find('.')) offset = 0;
		value.erase(value.find_last_not_of('0') + offset, string::npos);
		break;
	case VT_String:
		value = freezeEntries[index].stringValue;
		break;
	}
	return value;
}

// Write cheat entries to memory
void submitValues(int index, HANDLE hProcess, vector<string>* textInputs) {
	string ti = (*textInputs)[index];
	switch (cheatEntries[index].variableType) {
	case Byte:
		writeByte(hProcess, cheatEntries[index].address, stoi(ti));
		break;
	case Byte2BigEndian:
		write2ByteBigEndian(hProcess, cheatEntries[index].address, stoi(ti));
		break;
	case Byte4BigEndian:
		write4ByteBigEndian(hProcess, cheatEntries[index].address, stoi(ti));
		break;
	case FloatBigEndian:
		writeFloatBigEndian(hProcess, cheatEntries[index].address, stof(ti));
		break;
	case String:
		writeString(hProcess, cheatEntries[index].address, ti.c_str(), ti.size());
		break;
	}
}

// Write frozen cheat entries to memory
void submitValuesFrozen(int index, HANDLE hProcess, vector<string>* textInputs) {
	index -= cheatEntries.size();
	string ti = (*textInputs)[index + cheatEntries.size()];
	switch (freezeEntries[index].variableType) {
	case Byte:
		writeByte(hProcess, freezeEntries[index].address, stoi(ti));
		break;
	case Byte2BigEndian:
		write2ByteBigEndian(hProcess, freezeEntries[index].address, stoi(ti));
		break;
	case Byte4BigEndian:
		write4ByteBigEndian(hProcess, freezeEntries[index].address, stoi(ti));
		break;
	case FloatBigEndian:
		writeFloatBigEndian(hProcess, freezeEntries[index].address, stof(ti));
		break;
	case String:
		writeString(hProcess, freezeEntries[index].address, ti.c_str(), ti.size());
		break;
	}
}

void buttonEvent(button_t* button, const SDL_Event* ev) {
	int w, h;
	TTF_SizeText(button->font, button->text, &w, &h);
	w += 40;
	h += 20;
	if (ev->type == SDL_MOUSEBUTTONDOWN) {
		if (ev->button.button == SDL_BUTTON_LEFT && ev->button.x >= button->x && ev->button.x <= button->x + w && ev->button.y >= button->y && ev->button.y <= button->y + h) {
			button->pressed = true;
		}
	}
}

bool button(SDL_Renderer* renderer, button_t* button) {
	int w, h;
	TTF_SizeText(button->font, button->text, &w, &h);
	w += 40;
	h += 20;
	SDL_Rect rect;
	rect.x = button->x;
	rect.y = button->y;
	rect.w = w;
	rect.h = h;
	int colorAdd = 0;
	int mx, my;
	SDL_GetMouseState(&mx, &my);
	if (mx >= button->x && mx <= button->x + w && my >= button->y && my <= button->y + h) {
		button->mouseover = true;
	}
	else {
		button->mouseover = false;
	}
	if (button->mouseover) {
		colorAdd = 20;
	}
	SDL_SetRenderDrawColor(renderer, button->color.r + colorAdd, button->color.g + colorAdd, button->color.b + colorAdd, button->color.a);
	SDL_RenderFillRect(renderer, &rect);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, button->color.a);
	SDL_RenderDrawRect(renderer, &rect);
	printText(renderer, button->font, button->text, button->x + 20, button->y + 10, button->textColor);

	if (button->pressed) {
		button->pressed = false;
		return true;
	}
	return false;
}

void toggleEvent(button_t* button, const SDL_Event* ev) {
	if (ev->type == SDL_MOUSEBUTTONDOWN) {
		if (ev->button.button == SDL_BUTTON_LEFT && ev->button.x >= button->x && ev->button.x <= button->x + button->w && ev->button.y >= button->y && ev->button.y <= button->y + button->h) {
			button->pressed = true;
		}
	}
}

bool toggle(SDL_Renderer* renderer, button_t* button, bool enabled) {
	SDL_Rect rect;
	rect.x = button->x;
	rect.y = button->y;
	rect.w = button->w;
	rect.h = button->h;
	int colorAdd = 0;
	int mx, my;
	SDL_GetMouseState(&mx, &my);
	if (mx >= button->x && mx <= button->x + button->w && my >= button->y && my <= button->y + button->h) {
		button->mouseover = true;
	} else {
		button->mouseover = false;
	}
	if (button->mouseover) {
		colorAdd = 20;
	}
	SDL_SetRenderDrawColor(renderer, button->color.r + colorAdd, button->color.g + colorAdd, button->color.b + colorAdd, button->color.a);
	SDL_RenderFillRect(renderer, &rect);
	SDL_SetRenderDrawColor(renderer, button->color.r - 50, button->color.g - 50, button->color.b - 50, button->color.a);
	SDL_RenderDrawRect(renderer, &rect);
	if (enabled) {
		rect.x += 2;
		rect.y += 2;
		rect.w -= 4;
		rect.h -= 4;
		SDL_SetRenderDrawColor(renderer, 50, 50, 50, button->color.a);
		SDL_RenderFillRect(renderer, &rect);
	}

	if (button->pressed) {
		button->pressed = false;
		return true;
	}
	return false;
}

// Freeze values on a different thread since reading/writing process memory is kind of slow
void freezeThread(unsigned long pid, HANDLE hProcess, vector<string>* textInputs) {
	while (!quit) {
		for (int i = 0; i < freezeEntries.size(); i++) {
			string ti = (*textInputs)[i + cheatEntries.size()];
			if (freezeEntries[i].active) {
				if (isDigits(ti)) {
					switch (freezeEntries[i].variableType) {
					case Byte:
						writeByte(hProcess, freezeEntries[i].address, stoi(ti));
						break;
					case Byte2BigEndian:
						write2ByteBigEndian(hProcess, freezeEntries[i].address, stoi(ti));
						break;
					case Byte4BigEndian:
						write4ByteBigEndian(hProcess, freezeEntries[i].address, stoi(ti));
						break;
					case FloatBigEndian:
						writeFloatBigEndian(hProcess, freezeEntries[i].address, stof(ti));
						break;
					case String:
						writeString(hProcess, freezeEntries[i].address, ti.c_str(), ti.size());
						break;
					}
				}
			} else if(windowShown) {
				loadValueFrozen(hProcess, i);
				string value = getStringValueFrozen(i + cheatEntries.size());
				(*textInputs)[i + cheatEntries.size()] = value;
			}
		}
		Sleep(freezeUpdate);
	}
}

void sdlWindow(unsigned long pid, HANDLE hProcess) {
	SDL_Window* window;
	SDL_Init(SDL_INIT_EVERYTHING);

	HWND parentWindow = findMainWindow(pid);
	RECT parentRect;
	GetClientRect(parentWindow, &parentRect);

	window = SDL_CreateWindow("Ch3at", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 480, parentRect.bottom - parentRect.top, SDL_WINDOW_SHOWN | SDL_WINDOW_BORDERLESS | SDL_WINDOW_ALWAYS_ON_TOP);

	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer == NULL) {
		SDL_DestroyWindow(window);
	}
	SDL_StopTextInput();

	SDL_SysWMinfo info;
	SDL_VERSION(&info.version);
	SDL_GetWindowWMInfo(window, &info);
	HWND hwnd = info.info.win.window;
	HDC hdc = GetDC(hwnd);
	SDL_EventState(SDL_DROPFILE, SDL_DISABLE); // Enabling this causes random exceptions? i dont get why

	if (!IsWindow(parentWindow)) {
		int choice = MessageBoxA(hwnd, "Skate 3 is not currently running.", "CH3AT Error", MB_OK | MB_ICONERROR);
		if (choice == IDCANCEL) quit = 1;
	}

	TTF_Init();
	TTF_Font* superFont = nullptr;
	superFont = TTF_OpenFont("fonts/unispace_bd.ttf", 30);

	TTF_Font* font = nullptr;
	font = TTF_OpenFont("fonts/dos_vga.ttf", 16);

	SetParent(hwnd, parentWindow);

	vector<string> textInputs;
	int selectedInput = 0;
	for (int i = 0; i < cheatEntries.size(); i++) {
		string value;
		value = getStringValue(i);
		
		textInputs.push_back(value);
	}
	for (int i = 0; i < freezeEntries.size(); i++) {
		string value;
		value = getStringValueFrozen(i + cheatEntries.size());

		textInputs.push_back(value);
		freezeEntries[i].active = false;
	}

	vector<button_t> buttons;
	button_t openButton = {
		10,
		10,
		0,
		0,
		(char*)"Source Code",
		font,
		{ 0, 0, 0 },
		{ 200, 200, 200, 255 }
	};
	buttons.push_back(openButton);
	button_t donateButton = {
		10,
		10,
		0,
		0,
		(char*)"Donate",
		font,
		{ 0, 0, 0 },
		{ 200, 200, 200, 255 }
	};
	buttons.push_back(donateButton);
	button_t closeButton = {
		110,
		10,
		0,
		0,
		(char*)"Close CH3AT Process",
		font,
		{ 0, 0, 0 },
		{ 200, 200, 200, 255 }
	};
	buttons.push_back(closeButton);
	button_t restoreButton = {
		155,
		10,
		0,
		0,
		(char*)"Restore Defaults",
		font,
		{ 0, 0, 0 },
		{ 200, 200, 200, 255 }
	};
	buttons.push_back(restoreButton);

	vector<button_t> toggles;
	button_t toggleButton = {
		450,
		0,
		20,
		20,
		(char*)" ",
		font,
		{ 0, 0, 0 },
		{ 200, 200, 200, 255 }
	};
	for (int i = 0; i < freezeEntries.size(); i++)
		toggles.push_back(toggleButton);

	thread freeze(freezeThread, pid, hProcess, &textInputs);

	SDL_Event e;
	int scroll = 0;
	bool hotkeyPressed = 0;
	int cursorLocation = 0;
	bool textInput = false;
	int scrollStep = 40;

	while (!quit) {
		if (!IsWindow(parentWindow))
			quit = 1;

		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_KEYDOWN) {
				if(IsWindowVisible(hwnd)) {
					if(textInput) {
						if (e.key.keysym.sym == SDLK_BACKSPACE && textInputs[selectedInput].length() > 0 && cursorLocation > 0) {
							textInputs[selectedInput].erase(textInputs[selectedInput].begin() + (cursorLocation - 1));
							cursorLocation--;
							if (cursorLocation < 0) cursorLocation = 0;
						} else if(e.key.keysym.sym == SDLK_c && SDL_GetModState() & KMOD_CTRL) {
							SDL_SetClipboardText(textInputs[selectedInput].c_str());
						} else if(e.key.keysym.sym == SDLK_v && SDL_GetModState() & KMOD_CTRL) {
							textInputs[selectedInput] = SDL_GetClipboardText();
						} else if(e.key.keysym.sym == SDLK_LEFT) {
							cursorLocation--;
							if (cursorLocation < 0) cursorLocation = 0;
						} else if(e.key.keysym.sym == SDLK_RIGHT) {
							cursorLocation++;
							if (cursorLocation > textInputs[selectedInput].size()) cursorLocation = textInputs[selectedInput].size();
						} else if(e.key.keysym.sym == SDLK_RETURN) {
							textInput = false;
							SDL_StopTextInput();
							if(selectedInput > cheatEntries.size() - 1)
								submitValuesFrozen(selectedInput, hProcess, &textInputs);
							else
								submitValues(selectedInput, hProcess, &textInputs);
						} else if(e.key.keysym.sym == SDLK_TAB) {
							if (selectedInput > cheatEntries.size() - 1)
								submitValuesFrozen(selectedInput, hProcess, &textInputs);
							else
								submitValues(selectedInput, hProcess, &textInputs);
							selectedInput++;
							cursorLocation = textInputs[selectedInput].size();
						}
					}
					if (e.key.keysym.sym == SDLK_DOWN) {
						scroll += scrollStep;
						if (scroll < 0) scroll = 0;
						if (scroll > 24 * textInputs.size() - (parentRect.bottom - parentRect.top) + 170) scroll = 24 * textInputs.size() - (parentRect.bottom - parentRect.top) + 170;
					} else if (e.key.keysym.sym == SDLK_UP) {
						scroll -= scrollStep;
						if (scroll < 0) scroll = 0;
						if (scroll > 24 * textInputs.size() - (parentRect.bottom - parentRect.top) + 170) scroll = 24 * textInputs.size() - (parentRect.bottom - parentRect.top) + 170;
					}
				}
			} else if(e.type == SDL_TEXTINPUT) {
				if (!(SDL_GetModState() & KMOD_CTRL && (e.text.text[0] == 'c' || e.text.text[0] == 'C' || e.text.text[0] == 'v' || e.text.text[0] == 'V')) && e.text.text[0] != '`' && IsWindowVisible(hwnd)) {
					string tmp(textInputs[selectedInput]);
					int tmpCL = cursorLocation;
					for (int i = 0; i < string(e.text.text).size(); i++) {
						tmp.insert(tmp.begin() + cursorLocation, 1, e.text.text[i]);
						cursorLocation++;
					}
					if (selectedInput > cheatEntries.size() - 1) {
						if (freezeEntries[selectedInput - cheatEntries.size()].valueType != VT_String) {
							if (isDigits(tmp))
								textInputs[selectedInput].assign(tmp);
							else
								cursorLocation = tmpCL;
						} else {
							textInputs[selectedInput].assign(tmp);
						}
					} else {
						if (cheatEntries[selectedInput].valueType != VT_String) {
							if (isDigits(tmp))
								textInputs[selectedInput].assign(tmp);
							else
								cursorLocation = tmpCL;
						} else {
							textInputs[selectedInput].assign(tmp);
						}
					}
				}
			} else if(e.type == SDL_MOUSEWHEEL) {
				if(IsWindowVisible(hwnd)) {
					if (e.wheel.y > 0) {
						scroll -= scrollStep;
					} else if(e.wheel.y < 0) {
						scroll += scrollStep;
					}
					if (scroll < 0) scroll = 0;
					if (scroll > 24 * textInputs.size() - (parentRect.bottom - parentRect.top) + 170) scroll = 24 * textInputs.size() - (parentRect.bottom - parentRect.top) + 170;
				}
			} else if(e.type == SDL_QUIT) {
				quit = true;
			}

			for (int i = 0; i < cheatEntries.size(); i++) {
				SDL_Rect tbRect;
				tbRect.x = 300;
				tbRect.y = 24 * i + 70 - scroll;
				tbRect.w = 170;
				tbRect.h = 20;
				int state = textBoxEvent(tbRect, &e);
				if (state == 1) {
					selectedInput = i;
					submitValues(selectedInput, hProcess, &textInputs);
					textInput = true;
					SDL_StartTextInput();
					cursorLocation = textInputs[selectedInput].size();
				} else if(state == 2) {
					textInputs[selectedInput] = getStringValue(selectedInput);
					cursorLocation = textInputs[selectedInput].size();
					submitValues(selectedInput, hProcess, &textInputs);
				}
			}

			for (int i = 0; i < freezeEntries.size(); i++) {
				SDL_Rect tbRect;
				tbRect.x = 300;
				tbRect.y = 24 * (i + cheatEntries.size()) + 70 - scroll;
				tbRect.w = 145;
				tbRect.h = 20;
				int state = textBoxEvent(tbRect, &e);
				if (state == 1) {
					selectedInput = (i + cheatEntries.size());
					submitValuesFrozen(selectedInput, hProcess, &textInputs);
					textInput = true;
					SDL_StartTextInput();
					cursorLocation = textInputs[selectedInput].size();
				} else if (state == 2) {
					textInputs[selectedInput] = getStringValueFrozen(selectedInput);
					cursorLocation = textInputs[selectedInput].size();
					submitValuesFrozen(selectedInput, hProcess, &textInputs);
				}
			}

			for (int i = 0; i < buttons.size(); i++) {
				buttonEvent(&buttons[i], &e);
			}
			for (int i = 0; i < toggles.size(); i++) {
				toggles[i].y = 24 * (i + cheatEntries.size()) + 70 - scroll;
				toggleEvent(&toggles[i], &e);
			}
		}

		int w, h;
		SDL_GetWindowSize(window, &w, &h);
		GetClientRect(parentWindow, &parentRect);
		if (windowShown) {
			SDL_SetWindowSize(window, w, parentRect.bottom - parentRect.top);
			SetWindowPos(hwnd, HWND_TOP, 0, 0, w, parentRect.bottom - parentRect.top, SWP_NOZORDER | SWP_NOACTIVATE);
			SDL_RenderClear(renderer);
		}

		if (GetAsyncKeyState(VK_OEM_3)) {
			hotkeyPressed = 1;
		} else if (hotkeyPressed) {
			hotkeyPressed = 0;
			if (IsWindowVisible(hwnd)) {
				SetWindowPos(hwnd, HWND_TOP, 0, 0, w, parentRect.bottom - parentRect.top, SWP_NOZORDER | SWP_NOACTIVATE | SWP_HIDEWINDOW);
				BringWindowToTop(parentWindow);
				SDL_StopTextInput();
				textInput = false;
				windowShown = false;
			} else {
				SetWindowPos(hwnd, HWND_TOP, 0, 0, w, parentRect.bottom - parentRect.top, SWP_NOZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW);
				BringWindowToTop(hwnd);
				SDL_StopTextInput();
				textInput = false;
				windowShown = true;
			}
		}

		if (IsWindowVisible(hwnd)) {
			int mx, my;
			SDL_GetMouseState(&mx, &my);
			for (int i = 0; i < cheatEntries.size(); i++) {
				printText(renderer, font, cheatEntries[i].description.substr(1, cheatEntries[i].description.size() - 2).c_str(), 10, 24 * i + 70 - scroll, { 255, 255, 255 });
				SDL_Rect tbRect;
				tbRect.x = 300;
				tbRect.y = 24 * i + 70 - scroll;
				tbRect.w = 170;
				tbRect.h = 20;
				SDL_Color tbColor = { 200, 200, 200 };
				if (mx >= tbRect.x && mx <= tbRect.x + tbRect.w && my >= tbRect.y && my <= tbRect.y + tbRect.h)
					tbColor = { 230, 230, 230 };
				drawTextbox(renderer, font, textInputs[i].c_str(), tbRect, tbColor);
			}
			for (int i = 0; i < freezeEntries.size(); i++) {
				int yPos = 24 * (i + cheatEntries.size()) + 70 - scroll;
				printText(renderer, font, freezeEntries[i].description.c_str(), 10, yPos, { 255, 255, 255 });
				SDL_Rect tbRect;
				tbRect.x = 300;
				tbRect.y = yPos;
				tbRect.w = 145;
				tbRect.h = 20;
				toggles[i].y = 24 * (i + cheatEntries.size()) + 70 - scroll;
				SDL_Color tbColor = { 200, 200, 200 };
				if (mx >= tbRect.x && mx <= tbRect.x + tbRect.w && my >= tbRect.y && my <= tbRect.y + tbRect.h)
					tbColor = { 230, 230, 230 };
				drawTextbox(renderer, font, textInputs[i + cheatEntries.size()].c_str(), tbRect, tbColor);
				if (toggle(renderer, &toggles[i], freezeEntries[i].active)) {
					freezeEntries[i].active = !freezeEntries[i].active;
				}
			}

			SetCursor(LoadCursor(NULL, IDC_ARROW));
			for (int i = 0; i < buttons.size(); i++) {
				if (buttons[i].mouseover)
					SetCursor(LoadCursor(NULL, IDC_HAND));
			}

			int sectionOffset = textInputs.size() * 24 - scroll;

			//printText(renderer, font, "Extras", 10, sectionOffset, { 255, 255, 255 });

			//sectionOffset += 100;

			buttons[0].y = sectionOffset + 80;
			if (button(renderer, &buttons[0])) {
				system(string("start https://github.com/ckosmic").c_str());
			}
			buttons[1].y = sectionOffset + 125;
			if (button(renderer, &buttons[1])) {
				system(string("start https://paypal.me/ckosmic").c_str());
			}
			buttons[2].y = sectionOffset + 125;
			if (button(renderer, &buttons[2])) {
				quit = 1;
			}
			buttons[3].y = sectionOffset + 80;
			if (button(renderer, &buttons[3])) {
				for (int i = 0; i < textInputs.size(); i++) {
					textInputs[i] = getStringValue(i);
					submitValues(i, hProcess, &textInputs);
				}
			}

			if (textInput) {
				SDL_Rect cursorRect = { 308 + cursorLocation * 9, 24 * selectedInput + 72 - scroll, 1, 16 };
				SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
				SDL_RenderFillRect(renderer, &cursorRect);
			}

			SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
			SDL_Rect titleRect = { 0, 0, w, 60 };
			SDL_RenderFillRect(renderer, &titleRect);
			printText(renderer, superFont, "CH3AT", 10, 10, { 255, 255, 255 });
			printText(renderer, font, "by ckosmic", 110, 27, { 255, 255, 255 });
			printText(renderer, font, ("version " + string(VERSION)).c_str(), 350, 27, { 255, 255, 255 });

			SDL_RenderPresent(renderer);
		}

		char buffer[256];
		GetWindowTextA(parentWindow, buffer, 256);
		string bufferStr = string(buffer);
		if (bufferStr.find(" | CH3AT") == string::npos) {
			SetWindowTextA(parentWindow, (string(buffer) + " | CH3AT").c_str());
		}
	}

	SDL_DestroyWindow(window);
	SDL_Quit();
	return;
}