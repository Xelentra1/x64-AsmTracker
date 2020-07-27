#pragma once
#include <vector>
#include <unordered_map>

//dbg API
class CRegisterTrace;

enum MATH_OP {
	NOP,
	SET,
	MEM,
	MEM_REG,
	ADD,
	SUB,
	IMUL,
	XOR,
	NOT,
	AND,
	SHR,
	SHL,
	BSWAP,
	ALIAS,
	FORMULA,
	REGISTER,
};
struct ASM_OP {
	MATH_OP op = { NOP };
	CRegisterTrace* r0 = NULL;
	CRegisterTrace* r1 = NULL;
	CRegisterTrace* formula = NULL;
	std::string alias;
	DWORD64 iValue = 0;
	bool mem_base = 0;
};


class CRegisterTrace {
public:
	std::vector<CRegisterTrace> vTrace;
	CRegisterTrace() {
		vTrace.clear();
	}
	ZydisRegister r = 0;
	DWORD64 rva;
	ASM_OP op;
	bool formulaPrinted = false;
	DWORD opCount();
	DWORD count();
	CRegisterTrace* get_prev();
	std::string get_operation();
	DWORD get_formulas(std::vector< CRegisterTrace*>* vFormulas);
	std::string get_formula(std::vector< CRegisterTrace*>* vFormulas);
};



class CRegisterFrame {
public:
	static DWORD c_idx;
	DWORD idx;
	DWORD64 rva = 0;
	CONTEXT ctx;
	DWORD iComment = 0;
	std::string comment;
	char szInstruction[64] = { 0 };
	CRegisterFrame(DWORD _rva, CONTEXT _ctx) : rva(_rva), ctx(_ctx) {
		idx = c_idx++;
	}
	CRegisterFrame(CONTEXT _ctx) : ctx(_ctx) {
		idx = c_idx++;
		rva = ctx.Rip - dbg.procBase;
	}
	ZydisDecodedInstruction get_instruction() {
		// Initialize decoder context
		ZydisDecoder decoder;
		ZydisDecoderInit(&decoder, ZYDIS_MACHINE_MODE_LONG_64, ZYDIS_ADDRESS_WIDTH_64);


		ZydisDecodedInstruction instruction;
		BYTE bRead[20];
		dbg.ReadTo(ctx.Rip, bRead, 20);

		if (ZYDIS_SUCCESS(ZydisDecoderDecodeBuffer(
			&decoder, bRead, 20,
			ctx.Rip, &instruction))) {
			if (!szInstruction[0]) {

				ZydisFormatter formatter;
				ZydisFormatterInit(&formatter, ZYDIS_FORMATTER_STYLE_INTEL);
				ZydisFormatterFormatInstruction(&formatter, &instruction, szInstruction, 64);
			}
		}
		return instruction;
	}
};

class CRegisterTracker {
public:
	std::unordered_map<ZydisRegister, std::string> register_alias;
	std::vector<CRegisterFrame> frames;
	CRegisterTracker() {
		frames = std::vector<CRegisterFrame>();
	}
	CRegisterTrace* track(ZydisRegister r, DWORD idx = -1);
};
extern CRegisterTracker* regTracker;

//UI
class CScriptGUI {
public:
	HWND hWnd;
	HWND hLogEdit;
	HWND hScriptEdit;
	HWND hScriptBox;
	void Init();
};
extern CScriptGUI* scriptGui;