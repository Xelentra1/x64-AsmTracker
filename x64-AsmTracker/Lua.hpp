#pragma once
#include "UI.h"
#include "sol2.hpp"
#pragma comment(lib,"LuaJIT/lib64/Release/LuaJIT.lib")

std::unique_ptr<sol::state> state;
#define lua (*state)


void LuaInit() {
	static bool bInit = false;
	if (bInit) return;
	bInit = true;
	//x = 0;
	state.reset(new sol::state);
	lua.open_libraries();

	auto ctx = lua.new_usertype<CONTEXT>("CONTEXT");
	ctx["rip"] = &CONTEXT::Rip;
	lua.set_function("FileLoaded", []() {
		return !dbg.filename.empty();
		});
	lua.set_function("Scan", [](std::string str) {
		return 0;
		});
	lua.set_function("SetRva", [](DWORD rva) {
		return 0;
		});
	lua.set_function("StepOver", [](sol::optional<DWORD> iSteps) {
		return 0;
		});
	lua.set_function("GetContext", []() {
		CONTEXT c = dbg.GetContext();
		return c;
		});
	lua.set_function("Log", [](std::string str) {
		char szPrev[1024];
		GetWindowTextA(scriptGui->hLogEdit, szPrev, 1024);
		str += "\r\n";
		strcat_s(szPrev,1024, str.c_str());
		SetWindowText(scriptGui->hLogEdit, szPrev);
		});
	lua.set_function("LoadPreviousFile", []() {
		extern void LoadPreviousFile();
		LoadPreviousFile();
		});

	printf("Lua Init!\n");
}

void ExecuteLua(char* szScript) {
	LuaInit();
	SetWindowText(scriptGui->hLogEdit, "");//clear

	try {
		sol::protected_function_result result = lua.do_string(szScript);
		if (!result.valid()) {
			sol::error e = result;
			throw e;
		}
	}
	catch (sol::error const& e) {
		printf("Execute Error %s\n", e.what());
		OutputDebugStringA(e.what());
	}
}