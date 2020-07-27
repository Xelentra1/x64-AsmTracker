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
extern void LoadFile(std::string);
extern void LoadPreviousFile();
extern void ResetDbg();
extern void SetAlias(DWORD reg, std::string str);
std::string DumpFormula(CRegisterTrace* r0_track);


#include <vector>
#define SIZE_OF_NT_SIGNATURE (sizeof(DWORD))
#define PEFHDROFFSET(a) (PIMAGE_FILE_HEADER)((LPVOID)((BYTE *)a + ((PIMAGE_DOS_HEADER)a)->e_lfanew + SIZE_OF_NT_SIGNATURE))
#define SECHDROFFSET(ptr) (PIMAGE_SECTION_HEADER)((LPVOID)((BYTE *)(ptr)+((PIMAGE_DOS_HEADER)(ptr))->e_lfanew+SIZE_OF_NT_SIGNATURE+sizeof(IMAGE_FILE_HEADER)+sizeof(IMAGE_OPTIONAL_HEADER)))

PIMAGE_SECTION_HEADER getCodeSection(LPVOID lpHeader) {
	PIMAGE_FILE_HEADER pfh = PEFHDROFFSET(lpHeader);
	if (pfh->NumberOfSections < 1)
	{
		return NULL;
	}
	PIMAGE_SECTION_HEADER psh = SECHDROFFSET(lpHeader);
	return psh;
}
size_t replace_all(std::string& str, const std::string& from, const std::string& to) {
	size_t count = 0;

	size_t pos = 0;
	while ((pos = str.find(from, pos)) != std::string::npos) {
		str.replace(pos, from.length(), to);
		pos += to.length();
		++count;
	}

	return count;
}

bool is_hex_char(const char& c) {
	return ('0' <= c && c <= '9') || ('a' <= c && c <= 'f') || ('A' <= c && c <= 'F');
}
std::vector<int> pattern(std::string patternstring) {
	std::vector<int> result;
	const uint8_t hashmap[] = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ........
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ........
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ........
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ........
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //  !"#$%&'
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ()*+,-./
		0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, // 01234567
		0x08, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 89:;<=>?
		0x00, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x00, // @ABCDEFG
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // HIJKLMNO
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // PQRSTUVW
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // XYZ[\]^_
		0x00, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x00, // `abcdefg
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // hijklmno
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // pqrstuvw
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // xyz{|}~.
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ........
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ........
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ........
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ........
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ........
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ........
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ........
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ........
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ........
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ........
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ........
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ........
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ........
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ........
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ........
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00  // ........
	};
	replace_all(patternstring, "??", " ? ");
	replace_all(patternstring, "?", " ?? ");
	replace_all(patternstring, " ", "");
	//boost::trim(patternstring);
	//assert(patternstring.size() % 2 == 0);
	for (std::size_t i = 0; i < patternstring.size() - 1; i += 2) {
		if (patternstring[i] == '?' && patternstring[i + 1] == '?') {
			result.push_back(0xFFFF);
			continue;
		}
		//assert(is_hex_char(patternstring[i]) && is_hex_char(patternstring[i + 1]));
		result.push_back((uint8_t)(hashmap[patternstring[i]] << 4) | hashmap[patternstring[i + 1]]);
	}
	return result;
}

std::vector<std::size_t> find_pattern(const uint8_t* data, std::size_t data_size, const std::vector<int>& pattern,bool bSingle = false) {
	// simple pattern searching, nothing fancy. boyer moore horsepool or similar can be applied here to improve performance
	std::vector<std::size_t> result;
	for (std::size_t i = 0; i < data_size - pattern.size() + 1; i++) {
		std::size_t j;
		for (j = 0; j < pattern.size(); j++) {
			if (pattern[j] == 0xFFFF) {
				continue;
			}
			if (pattern[j] != data[i + j]) {
				break;
			}
		}
		if (j == pattern.size()) {
			result.push_back(i);
			if (bSingle) break;
		}
	}
	return result;
}
std::vector<DWORD64> AOBScan(std::string str_pattern,bool bSingle = false) {
	std::vector<DWORD64> ret;
	HANDLE hProc = dbg.debuggeehProcess;

	ULONG_PTR dwStart = dbg.procBase;

	LPVOID lpHeader = malloc(0x1000);
	ReadProcessMemory(hProc, (LPCVOID)dwStart, lpHeader, 0x1000, NULL);

	DWORD delta = 0x1000;
	LPCVOID lpStart = 0; //0
	DWORD nSize = 0;// 0x548a000;

	PIMAGE_SECTION_HEADER SHcode = getCodeSection(lpHeader);
	if (SHcode) {
		nSize = SHcode->Misc.VirtualSize;
		delta = SHcode->VirtualAddress;
		lpStart = ((LPBYTE)dwStart + delta);
	}
	if (nSize) {

		LPVOID lpCodeSection = malloc(nSize);
		ReadProcessMemory(hProc, lpStart, lpCodeSection, nSize, NULL);

		//sprintf_s(szPrint, 124, "Size: %i / Start:%p / Base: %p", nSize, dwStart,lpStart);
		//MessageBoxA(0, szPrint, szPrint, 0);
		//
		auto res = find_pattern((const uint8_t*)lpCodeSection, nSize, pattern(str_pattern.c_str()), bSingle);
		ret = res;
		for (UINT i = 0; i < ret.size(); i++) {
			ret[i] += delta;
		}

		free(lpCodeSection);
	}
	else {
		printf("bad .code section.\n");
	}
	free(lpHeader);


	return ret;
}
DWORD DoScan(std::string pattern, DWORD offset = 0, DWORD base_offset = 0, DWORD pre_base_offset = 0, DWORD rIndex = 0) {
	//ULONG_PTR dwBase = (DWORD_PTR)GetModuleHandleW(NULL);
	auto r = AOBScan(pattern);
	if (!r.size())
		return 0;
	//char msg[124];
	//sprintf_s(msg,124,"%s ret %i\n",pattern.c_str(),r.size() );
	//OutputDebugStringA(msg);
	DWORD ret = r[rIndex] + pre_base_offset;
	if (offset == 0) {
		return ret + base_offset;
	}
	DWORD dRead = Read<DWORD>((LPBYTE)dbg.procBase + ret + offset);
	ret = ret + dRead + base_offset;
	//ret = ret + *(DWORD*)(dwBase + ret + offset) + base_offset;
	return ret;
}

ZydisDecodedInstruction Decode(DWORD64 rip) {

	ZydisDecoder decoder;
	ZydisDecoderInit(&decoder, ZYDIS_MACHINE_MODE_LONG_64, ZYDIS_ADDRESS_WIDTH_64);


	ZydisDecodedInstruction instruction;
	BYTE bRead[20];
	dbg.ReadTo(rip, bRead, 20);

	if (ZYDIS_SUCCESS(ZydisDecoderDecodeBuffer(
		&decoder, bRead, 20,
		rip, &instruction))) {
	}
	return instruction;
}

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

	tbl["rax"] = &CONTEXT::Rax;
	tbl["rcx"] = &CONTEXT::Rcx;
	tbl["rdx"] = &CONTEXT::Rdx;
	tbl["rbx"] = &CONTEXT::Rbx;
	tbl["rsp"] = &CONTEXT::Rsp;
	tbl["rbp"] = &CONTEXT::Rbp;
	tbl["rsi"] = &CONTEXT::Rsi;
	tbl["rdi"] = &CONTEXT::Rdi;
	tbl["r8"] = &CONTEXT::R8;
	tbl["r9"] = &CONTEXT::R9;
	tbl["r10"] = &CONTEXT::R10;
	tbl["r11"] = &CONTEXT::R11;
	tbl["r12"] = &CONTEXT::R12;
	tbl["r13"] = &CONTEXT::R13;
	tbl["r14"] = &CONTEXT::R14;
	tbl["r15"] = &CONTEXT::R15;

	auto rtbl = lua.new_usertype<CRegisterTrace>("RegisterTrace");
	rtbl["rva"] = &CRegisterTrace::rva;
	rtbl["dump"] = DumpFormula;


	lua.set_function("GetTickCount", GetTickCount);
	lua.set_function("ResetDbg", ResetDbg);
	lua.set_function("FileLoaded", []() {
		return !dbg.filename.empty();
		});
	lua.set_function("SetAlias", [](DWORD reg,std::string str) {
		//check if reg is a valid register..
		SetAlias(reg, str);
		});

	lua.set_function("GetBase", []() {
		return dbg.procBase;
		});
	lua.set_function("GetRelativeAddress", [](DWORD64 reg) {
		auto inst = Decode(reg);
		DWORD64 tVal = (inst.length + inst.operands[1].mem.disp.value + inst.instrAddress) - dbg.procBase;
		return tVal;
		});
	lua.set_function("Decode", [](DWORD64 reg, sol::protected_function callback) {
		bool bLoop = true;
		while (bLoop) {
			auto it = Decode(reg);
			auto ret = callback(reg,it);
			bLoop = ret.valid() && ret;
			reg += it.length;
		}
		});
	lua.set_function("Track", [](DWORD reg) {

		return regTracker->track(reg, regTracker->frames.size());
		});
	lua.set_function("DoScan", [](std::string str) {
		auto r = AOBScan(str, true);
		return r.size() ? r[0] : 0;
		});
	lua.set_function("DoScans", [](std::string str) {
		auto r = AOBScan(str);
		return sol::as_table(r);
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
	lua.set_function("SetContext", [](CONTEXT c) {
		dbg.SetContext(&c);
		});
	lua.set_function("Log", [](std::string str) {
		printf("log: %s\n", str.c_str());
#define MAX_LOG 1024*10
		char szPrev[MAX_LOG];
		GetWindowTextA(scriptGui->hLogEdit, szPrev, MAX_LOG);
		str += "\r\n";
		strcat_s(szPrev, MAX_LOG, str.c_str());
		SetWindowText(scriptGui->hLogEdit, szPrev);
		});
	lua.set_function("LoadFile", LoadFile);

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