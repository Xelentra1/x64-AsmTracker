#pragma once
#include "UI.h"
#include "sol2.hpp"
#pragma comment(lib,"LuaJIT/lib64/Release/LuaJIT.lib")

std::unique_ptr<sol::state> state;
#define lua (*state)

extern void ShowDisasm();
extern void ShowDisasm();
extern void ListTrace();
extern void StepOver();
extern void LoadPreviousFile();
extern void ResetDbg();
extern void SetAlias(DWORD reg, std::string str);
std::string DumpFormula(CRegisterTrace* r0_track);

void LuaInit() {
	static bool bInit = false;
	if (bInit) return;
	bInit = true;
	//x = 0;
	state.reset(new sol::state);
	lua.open_libraries();

	for (DWORD i = ZYDIS_REGISTER_RAX; i <= ZYDIS_REGISTER_R15; i++) {

		std::string str = ZydisRegisterGetString(i);
		std::transform(str.begin(), str.end(), str.begin(), ::toupper);
		lua[str] = i;
	}

	auto tbl = lua.new_usertype<CONTEXT>("CONTEXT");
	tbl["rip"] = &CONTEXT::Rip;

	auto rtbl = lua.new_usertype<CRegisterTrace>("RegisterTrace");
	rtbl["rva"] = &CRegisterTrace::rva;
	rtbl["dump"] = DumpFormula;


	lua.set_function("ResetDbg", ResetDbg);
	lua.set_function("FileLoaded", []() {
		return !dbg.filename.empty();
		});
	lua.set_function("SetAlias", [](DWORD reg,std::string str) {
		//check if reg is a valid register..
		SetAlias(reg, str);
		});
	lua.set_function("Track", [](DWORD reg) {

		return regTracker->track(reg,regTracker->frames.size());
		});
	lua.set_function("Scan", [](std::string str) {
		return 0;
		});
	lua.set_function("GetRva", []() {
		CONTEXT c = dbg.GetContext();
		return c.Rip - dbg.procBase;
		});
	lua.set_function("SetRva", [](DWORD rva) {
		CONTEXT c = dbg.GetContext();
		c.Rip = dbg.procBase + rva;
		dbg.SetContext(&c);
		ShowDisasm();
		});
	lua.set_function("StepOver", [](sol::optional<DWORD> optSteps) {
		DWORD iSteps = optSteps.value_or(1);
		for (DWORD i = 0; i < iSteps; i++) {
			StepOver();
		}
		ListTrace();
		});
	lua.set_function("GetContext", []() {
		CONTEXT c = dbg.GetContext();
		return c;
		});
	lua.set_function("Log", [](std::string str) {
#define MAX_LOG 1024*10
		char szPrev[MAX_LOG];
		GetWindowTextA(scriptGui->hLogEdit, szPrev, MAX_LOG);
		str += "\r\n";
		strcat_s(szPrev, MAX_LOG, str.c_str());
		SetWindowText(scriptGui->hLogEdit, szPrev);
		});
	lua.set_function("LoadPreviousFile", []() {
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