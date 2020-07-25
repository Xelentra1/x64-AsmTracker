#include <Windows.h>
#include <CommCtrl.h>

#pragma comment(lib,"Dbghelp.lib")
#pragma comment(lib,"Comctl32.lib")
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include <vector>
#include "Dbg.hpp"

HINSTANCE hInstance;
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
#define SCREEN_WIDTH  1000
#define SCREEN_HEIGHT 832


std::vector<std::string> vLastFiles;

void SaveCfg();


enum WND_MENU {
	FILTER_STATIC = 3000,
	FILTER_LABEL,
	FILTER_BUTTON,
	RIP_EDIT,
	STEPX_EDIT,
	CB_ALIAS,
	EDIT_ALIAS,
	ADD_ALIAS,
	STEP_OVER_BUTTON,
	PTR_STATIC,
	PTR_LABEL,
	PTR_BUTTON,
	SCAN_LISTBOX,
	SCAN_LISTBOX2,
	SCAN_LISTBOX3,
	SCAN_LISTBOX4,
	ALIAS_LISTBOX,
	LOG_BOX,
	SCAN_LISTVIEW,
	STATUS_STATIC,
	IDM_FILE_NEW,
	IDM_FILE_OPEN,
	IDM_FILE_QUIT

};
class CMainGUI {
public:
	HWND hWnd;
	HWND hStatic;
	HWND hListBox;
	HWND hListBox2;
	HWND hListBox3;
	HWND hListBox4;
	HWND hListBox5;
	HWND hwndEdit;

	HWND stepEdit;
	HWND ripEdit;
	HWND hAliasEdit;
	HWND hAliasCb;
	bool bFinish = false;
	void Init() {
		INITCOMMONCONTROLSEX icex;           // Structure for control initialization.
		icex.dwICC = ICC_LISTVIEW_CLASSES;
		InitCommonControlsEx(&icex);

		const char* wndClass = "wndclass";
		WNDCLASSEXA wc;
		ZeroMemory(&wc, sizeof(WNDCLASSEX));
		wc.cbSize = sizeof(WNDCLASSEX);
		//wc.style = CS_DBLCLKS | CS_GLOBALCLASS;// CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
		wc.lpfnWndProc = WindowProc;
		wc.lpszClassName = wndClass;
		wc.hInstance = hInstance;//GetModuleHandle(nullptr);
		//wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)(COLOR_WINDOW);//(HBRUSH)GetStockObject(BLACK_BRUSH);//reinterpret_cast<HBRUSH>(COLOR_WINDOW);
		//wc.lpszClassName = wndClass;
		RegisterClassExA(&wc);
		RECT wr = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
		AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);
		unsigned int dwStyle = (WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX);
		hWnd = CreateWindowExA(NULL, wndClass, "x64-ASM Tracker", dwStyle, 300, 300, wr.right - wr.left, wr.bottom - wr.top, nullptr, nullptr, GetModuleHandle(nullptr), nullptr);

		HMENU hMenubar;
		HMENU hMenu;

		hMenubar = CreateMenu();
		hMenu = CreateMenu();

		AppendMenuW(hMenu, MF_STRING, IDM_FILE_NEW, L"&New");
		AppendMenuW(hMenu, MF_STRING, IDM_FILE_OPEN, L"&Open");
		AppendMenuW(hMenu, MF_SEPARATOR, 0, NULL);
		AppendMenuW(hMenu, MF_STRING, IDM_FILE_QUIT, L"&Reset Frames");

		AppendMenuW(hMenubar, MF_POPUP, (UINT_PTR)hMenu, L"&File");
		SetMenu(hWnd, hMenubar);

		WPARAM wFont = WPARAM(GetStockObject(DEFAULT_GUI_FONT));

		hStatic = CreateWindowEx(0, WC_STATICA, NULL,
			WS_CHILD | WS_VISIBLE,
			20, 500, 280, 32,
			hWnd, (HMENU)STATUS_STATIC, hInstance, NULL);
		SetWindowText(hStatic, "Status: ");
		SendMessage(hStatic, WM_SETFONT, wFont, TRUE);


		 ripEdit = CreateWindowEx(0, WC_EDITA, NULL,
			WS_CHILD | WS_VISIBLE,
			80, 390, 62, 22,
			hWnd, (HMENU)RIP_EDIT, hInstance, NULL);
		SetWindowText(ripEdit, "0x14343DD");
		SendMessage(ripEdit, WM_SETFONT, wFont, TRUE);


		HWND hSearch = CreateWindowEx(0, WC_BUTTONA, NULL,
			WS_CHILD | WS_VISIBLE,
			180, 390, 82, 22,
			hWnd, (HMENU)FILTER_BUTTON, hInstance, NULL);
		SetWindowText(hSearch, "SET RIP");
		SendMessage(hSearch, WM_SETFONT, wFont, TRUE);


		stepEdit = CreateWindowEx(0, WC_EDITA, NULL,
			WS_CHILD | WS_VISIBLE,
			80, 420, 62, 22,
			hWnd, (HMENU)STEPX_EDIT, hInstance, NULL);
		SetWindowText(stepEdit, "32");
		SendMessage(stepEdit, WM_SETFONT, wFont, TRUE);

		hSearch = CreateWindowEx(0, WC_BUTTONA, NULL,
			WS_CHILD | WS_VISIBLE,
			180, 420, 82, 22,
			hWnd, (HMENU)STEP_OVER_BUTTON, hInstance, NULL);
		SetWindowText(hSearch, "STEP OVER");
		SendMessage(hSearch, WM_SETFONT, wFont, TRUE);


		HWND hGrpButtons = CreateWindowExW(WS_EX_WINDOWEDGE,
			L"BUTTON",
			L"DISASM",
			WS_VISIBLE | WS_CHILD | BS_GROUPBOX,
			8, 2,
			280, 380,
			hWnd,
			(HMENU)4,
			hInstance, NULL);
		SendMessage(hGrpButtons, WM_SETFONT, wFont, TRUE);

		hListBox = CreateWindowEx(0, WC_LISTBOXA, NULL,
			WS_CHILD | WS_VISIBLE | WS_VSCROLL | LBS_NOTIFY,
			18, 18, 260, 360,
			hWnd, (HMENU)SCAN_LISTBOX, hInstance, NULL);
		SendMessage(hListBox, WM_SETFONT, wFont, TRUE);


		 hGrpButtons = CreateWindowExW(WS_EX_WINDOWEDGE,
			L"BUTTON",
			L"DEBUG FRAMES",
			WS_VISIBLE | WS_CHILD | BS_GROUPBOX,
			302, 2,
			280, 380,
			hWnd,
			(HMENU)4,
			hInstance, NULL);
		SendMessage(hGrpButtons, WM_SETFONT, wFont, TRUE);

		hListBox2 = CreateWindowEx(0, WC_LISTBOXA, NULL,
			WS_CHILD | WS_VISIBLE | WS_VSCROLL | LBS_NOTIFY,
			308, 18, 260, 360,
			hWnd, (HMENU)SCAN_LISTBOX2, hInstance, NULL);
		SendMessage(hListBox2, WM_SETFONT, wFont, TRUE);


		hGrpButtons = CreateWindowExW(WS_EX_WINDOWEDGE,
			L"BUTTON",
			L"R0",
			WS_VISIBLE | WS_CHILD | BS_GROUPBOX,
			302, 392,
			280, 180,
			hWnd,
			(HMENU)4,
			hInstance, NULL);
		SendMessage(hGrpButtons, WM_SETFONT, wFont, TRUE);

		hListBox3 = CreateWindowEx(0, WC_LISTBOXA, NULL,
			WS_CHILD | WS_VISIBLE | WS_VSCROLL | LBS_NOTIFY,
			308, 406, 260, 160,
			hWnd, (HMENU)SCAN_LISTBOX3, hInstance, NULL);
		SendMessage(hListBox3, WM_SETFONT, wFont, TRUE);


		hGrpButtons = CreateWindowExW(WS_EX_WINDOWEDGE,
			L"BUTTON",
			L"R1",
			WS_VISIBLE | WS_CHILD | BS_GROUPBOX,
			590, 392,
			280, 180,
			hWnd,
			(HMENU)4,
			hInstance, NULL);
		SendMessage(hGrpButtons, WM_SETFONT, wFont, TRUE);

		hListBox4 = CreateWindowEx(0, WC_LISTBOXA, NULL,
			WS_CHILD | WS_VISIBLE | WS_VSCROLL | LBS_NOTIFY,
			598, 406, 260, 160,
			hWnd, (HMENU)SCAN_LISTBOX4, hInstance, NULL);
		SendMessage(hListBox4, WM_SETFONT, wFont, TRUE);


		hGrpButtons = CreateWindowExW(WS_EX_WINDOWEDGE,
			L"BUTTON",
			L"LOGS",
			WS_VISIBLE | WS_CHILD | BS_GROUPBOX,
			300, 580,
			640, 188,
			hWnd,
			(HMENU)4,
			hInstance, NULL);
		SendMessage(hGrpButtons, WM_SETFONT, wFont, TRUE);

		hwndEdit = CreateWindowExA(
			0, "EDIT",   // predefined class 
			NULL,         // no window title 
			WS_CHILD | WS_VISIBLE | WS_VSCROLL |
			ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL,
			308, 596, 620, 160,   // set size in WM_SIZE message 
			hWnd,         // parent window 
			(HMENU)LOG_BOX,   // edit control ID 
			hInstance,
			NULL);        // pointer not needed 

		SendMessage(hwndEdit, WM_SETFONT, wFont, TRUE);


		hGrpButtons = CreateWindowExW(WS_EX_WINDOWEDGE,
			L"BUTTON",
			L"ALIAS",
			WS_VISIBLE | WS_CHILD | BS_GROUPBOX,
			8, 580,
			280, 188,
			hWnd,
			(HMENU)4,
			hInstance, NULL);
		SendMessage(hGrpButtons, WM_SETFONT, wFont, TRUE);


		hListBox5= CreateWindowEx(0, WC_LISTBOXA, NULL,
			WS_CHILD | WS_VISIBLE | WS_VSCROLL | LBS_NOTIFY,
			18, 596, 260, 140,
			hWnd, (HMENU)ALIAS_LISTBOX, hInstance, NULL);
		SendMessage(hListBox5, WM_SETFONT, wFont, TRUE);

		hAliasCb = CreateWindow( WC_COMBOBOX, NULL,
			CBS_DROPDOWN | CBS_HASSTRINGS | WS_VISIBLE | WS_CHILD | WS_BORDER,
			12, 732, 62, 22,
			hWnd, (HMENU)CB_ALIAS, hInstance, NULL);

		for (DWORD i = ZYDIS_REGISTER_RAX; i <= ZYDIS_REGISTER_R15; i++) {
			SendMessage(
				hAliasCb,
				(UINT)CB_ADDSTRING,
				(WPARAM)0, (LPARAM)ZydisRegisterGetString(i));
		}
		SendMessage(hAliasCb, WM_SETFONT, wFont, TRUE);

		hAliasEdit = CreateWindowEx(0, WC_EDITA, NULL,
			WS_CHILD | WS_VISIBLE,
			112, 732, 62, 22,
			hWnd, (HMENU)EDIT_ALIAS, hInstance, NULL);
		SetWindowText(hAliasEdit, "alias");
		SendMessage(hAliasEdit, WM_SETFONT, wFont, TRUE);

		hSearch = CreateWindowEx(0, WC_BUTTONA, NULL,
			WS_CHILD | WS_VISIBLE,
			212, 732, 62, 22,
			hWnd, (HMENU)ADD_ALIAS, hInstance, NULL);
		SetWindowText(hSearch, "ADD");
		SendMessage(hSearch, WM_SETFONT, wFont, TRUE);


		MoveWindow(hWnd, GetSystemMetrics(SM_CXSCREEN)- SCREEN_WIDTH-80,80,SCREEN_WIDTH, SCREEN_HEIGHT, false);

		ShowWindow(hWnd, SW_SHOWNORMAL);
	}
	void Run() {
		MSG msg;
		while (!bFinish) {
			while (PeekMessage(&msg, hWnd, 0, 0, PM_REMOVE)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
				if (msg.message == WM_QUIT || msg.message == WM_DESTROY)
				{
					ExitProcess(0);
					bFinish = true;
					break;
				}
			}
			Sleep(10);
		}
	}

	void SetTitle(LPCSTR str) {

		SetWindowText(gui.hWnd, (std::string("x64-ASM Tracker - ") + str).c_str());
	}
	void SetStatus(LPCSTR str) {

		SetWindowText(gui.hStatic, str);
	}
	void SetLog(LPCSTR str) {

		SetWindowText(gui.hwndEdit, str);
	}
} gui;


class CRegisterFrame {
public:
	static DWORD c_idx;
	DWORD idx;
	DWORD64 rva = 0;
	CONTEXT ctx;
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
		}
		return instruction;
	}
};
DWORD CRegisterFrame::c_idx = 0;

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
template <class T>
bool VectorContains(std::vector<T> vec,T cmp) {
	bool bListed = false;
	for (DWORD k = 0; !bListed && k < vec.size(); k++) {
		if (cmp == vec[k]) bListed = true;
	}
	return bListed;
}
std::string find_alias(ZydisRegister r);
class CRegisterTrace {
public:
	std::vector<CRegisterTrace> vTrace;
	ZydisRegister r = 0;
	DWORD64 rva;
	ASM_OP op;
	bool formulaPrinted = false;
	DWORD opCount() {
		auto p = this;
		DWORD c = 0;
		while (p) {
			if(p->rva)c++;
			p = p->get_prev();
		}
		return c;
	}
	DWORD count() {
		//if ( op.op == ALIAS) return 0;
		DWORD i = 1;
		if (op.r1) i += op.r1->count();
		return i;
	}
	CRegisterTrace* get_prev() {
		return op.r0;
	}
	std::string get_operation() {
		std::string ret;

		switch (op.op) {
		case SET: {
			char buf[64];
			if (op.mem_base)
				sprintf_s(buf, 64, "(BASE_MEM+0x%X)", op.iValue);
			else
				sprintf_s(buf, 32, "0x%p", op.iValue);
			ret = buf;
			break;
		}
		case MEM: {
			char buf[64];
			//printf("mem %p / %p\n", rva,op.r1->rva);
			if (op.r1->op.op == REGISTER || op.r1->op.op == BSWAP) {
				sprintf_s(buf, 64, "[%s + 0x%X]", op.r1->get_operation().c_str(), op.iValue);
			}
			else {
				sprintf_s(buf, 64, "[F_%p + 0x%X]", op.r1->rva, op.iValue);

			}
			return buf;
			break;
		}
		case BSWAP:
			return "BSWAP";
			break;
		case ALIAS:
			return op.alias;
			break;
		case REGISTER: {
			//track register?
			if (!op.formula) {
				if (op.r0 && op.r0->count() > 1)
					return op.r0->get_operation();

				if (!rva) {
					//check if we have an alias..

					auto alias = find_alias(op.iValue);
					if (!alias.empty()) return alias;

					return op.alias;//alias is reg name
				}
				char buf[64];
				sprintf_s(buf, 64, "[%s / 0x%08X]", op.alias.c_str(), rva);
				return buf;
				//else return op.alias;
			}
			break;
		}
		case SUB:
			ret = "-";
			break;
		case ADD:
			ret = "+";
			if (op.iValue > 0 && op.r1 == NULL) {

				char buf[64];
				sprintf_s(buf, 64, "%i", op.iValue);
				ret += buf;
			}
			break;
		case NOT:
			ret = "~";
			break;
		case XOR:
			ret = "^";
			break;
		case SHR:
			char buf[124];
			//printf("%p has form? %p\n", rva, op.formula);
			if (op.formula) {
				sprintf_s(buf, 124, "F_%p >> 0x%02X", op.formula->rva, op.iValue);
				return buf;
			}
			if (op.r1->rva == 0) { //is reg
				sprintf_s(buf, 124, "%s >> 0x%02X", op.r1->op.alias.c_str(), op.iValue);
			}
			else {
				sprintf_s(buf, 124, "F_%p >> 0x%02X", op.r1->rva, op.iValue);
			}
			return buf;
			//if (op.r0->op.formula)ret += "F!";
			break;

		case IMUL:
			if (op.iValue > 0) {

				char buf[32];
				sprintf_s(buf, 32, " %i *", op.iValue);
				ret += buf;
			}
			else {
				ret = "*";
			}
			break;
		default:
			printf("unk at %p\n", rva);
			ret = "UNK " + std::to_string(op.op);
			break;
		}
		if (op.formula) {
			char buf[124];
			sprintf_s(buf, 124, "%s F_%p", ret.c_str(), op.formula->rva);
			return buf;// op.formula->get_operation();
		}
		if (op.r1) {
			/*if (!op.r1->rva) {
				char buf[124];
				sprintf_s(buf, 124, "FML %p", op.r1->rva);

				ret += buf;
			}
			else {*/
				ret += op.r1->get_operation();
			//}
		}
		return ret;
	}
	//something to iterate subformulas
	DWORD get_formulas(std::vector< CRegisterTrace*>* vFormulas) {
		DWORD c = 0;
		auto prev = this;
		while (prev) {
			c++;
			vFormulas->push_back(prev);
			if(prev!=this)prev->get_formulas(vFormulas);
			prev = prev->get_prev();
		}
		return c;
	}
	std::string get_formula(std::vector< CRegisterTrace*>* vFormulas) {
		std::string r;
		//if we have a previus formula, push to vector..
		auto op = this;
		std::vector< CRegisterTrace*> sub_f;
		bool endFml = false;
		DWORD iIsolate = 0;
		while (op && !endFml) {
			//printf("op %p / %p\n", op->rva, op);
			//printf("Ret: %s\n", r.c_str());
			//or use fnc..
			 {
				if (op->op.formula) { //also check if has been used previously!
					if (!VectorContains(*vFormulas, op->op.formula))vFormulas->push_back(op->op.formula);
					op->op.formula->get_formulas(&sub_f);

					op->op.formula->get_formula(vFormulas);

					for (DWORD i = 0; i < sub_f.size(); i++) {
						auto _op = sub_f[i];
						//printf("sub %p / %p\n", _op->rva, _op);
						std::vector< CRegisterTrace*> sub2;
						_op->op.formula->get_formulas(&sub2);
						for (DWORD j = 0; j < sub2.size(); j++) {
							auto _op2 = sub2[j];
							//printf("sub2 %p / %p\n", _op2->rva, _op2);
							if (_op2 == op->get_prev()) {
								//printf("IS FML!\n");

								if(!VectorContains(*vFormulas,_op2))vFormulas->push_back(_op2);

								char buf[124];
								sprintf_s(buf, 32, "F_%p ", _op2->rva);
								r = "("+std::string(buf)+ op->get_operation()+")"+r;


								for (DWORD i = 0; i < iIsolate; i++) r = "(" + r;
								return "(" + r + ")";
							}
						}
						if (_op == op) {
							vFormulas->push_back(op);
						}
					}
				}
				if (!endFml) {
					if (r.empty()) r = op->get_operation() + r;
					else {
						iIsolate++;
						r = op->get_operation() + ")" + r;
					}
				}
			}
			//op->get_formulas(&sub_f);
			op = op->get_prev();
		}
		for (DWORD i = 0; i < iIsolate; i++) r = "("+r;
		return "(" + r + ")";
	}
};
std::vector< CRegisterTrace*> vTraces;
#include <unordered_map>
class CRegisterTracker {
public:
	std::unordered_map<ZydisRegister, std::string> register_alias;
	std::vector<CRegisterFrame> frames;

	CRegisterTrace* track(ZydisRegister r, DWORD idx = -1) {
		if (r == ZYDIS_REGISTER_NONE) return NULL;
		CRegisterTrace* ret = new CRegisterTrace;
		ret->r = r;
		ret->op.r0 = 0;
		ret->op.r1 = 0;
		//check for alias..
		//auto alias = get_alias(r);
		DWORD64 pRip = 0;

		//iter frames..
		for (auto it = frames.rbegin(); it != frames.rend(); ++it)
		{
			if (idx !=-1&& it->idx >= idx) continue;
			//else if (it == frames.rbegin()) continue; //skip first.. //loop till cur frame..


			auto _inst = it->get_instruction();
			auto inst = &_inst;
			DWORD _idx = it->idx;
			if (inst) {
				if (inst->operands[0].reg.value == r ||
					(inst->mnemonic == ZYDIS_MNEMONIC_BSWAP && inst->operands[1].mem.base == r)) {

					pRip = inst->instrAddress;
					//printf("[%i > %i] track %s at %p\n", idx,_idx, ZydisRegisterGetString(r), pRip);
					ret->rva = pRip - dbg.procBase;

					if (inst->mnemonic == ZYDIS_MNEMONIC_BSWAP) {
						ret->op.op = BSWAP;
						//r0 is register..

						//auto r0_track = track(inst->operands[1].reg.value, _idx);
						//ret->op.r0 = r0_track;
					}
					if (inst->mnemonic == ZYDIS_MNEMONIC_MOVSXD) {
						//make sure to note its a WORD
						ret->op.op = REGISTER;
						//r1 is register, check if we use alias or formula

						ret->op.alias = ZydisRegisterGetString(inst->operands[1].reg.value);

						//printf("{\n");
						ret->op.r1 = track(inst->operands[1].reg.value, _idx);
						auto t = ret->op.r1;
						//printf("%p / %p / look for mov here! else r1 is going to be reg %i / %i\n",inst->instrAddress,ret->op.r1->rva, inst->operands[1].reg.value, ret->op.r1->op.iValue);
						//maybe set r0 to r1?
						delete ret;
						ret = t; //lets return the r1 trace
						return t;
					}
					else if (inst->mnemonic == ZYDIS_MNEMONIC_MOV) {
						ret->op.op = SET;
						/*if (inst->operands[0].reg.value == mainReg) { //overwrite main reg?
							ret->op.r0 = track2(inst->operands[1].reg.value, it->idx);
							//ret = track(inst->operands[1].reg.value, it->idx);
						}
						else {*/
						//check if its register, if so check alias
						if (inst->operands[1].type == ZYDIS_OPERAND_TYPE_MEMORY) {
							if (inst->operands[1].mem.base == ZYDIS_REGISTER_RIP) {
								ret->op.op = MEM;
								//ret->op.r0 = track2(inst->operands[0].reg.value, _idx);
								ret->op.r1 = track(inst->operands[1].mem.base, _idx - 1);//-1 in case our r0 reg is same as r0
								ret->op.iValue = inst->operands[1].mem.disp.value;
								printf("%p mem disp %i / %p\n", inst->instrAddress, inst->operands[1].mem.base, ret->op.formula);
								//ret = track(inst->operands[1].mem.base, it->idx);
							}
							else {
								ret->op.op = MEM;
								ret->op.iValue = inst->operands[1].mem.disp.value;
								ret->op.r1 = track(inst->operands[1].mem.base);
								ret->op.mem_base = true;
							}
						}
						else if (inst->operands[1].reg.value == ZYDIS_REGISTER_NONE) {
							ret->op.op = SET;
							ret->op.iValue = inst->operands[1].imm.value.u;
							ret->op.r1 = track(inst->operands[1].mem.base, _idx);
							char buf[32];
							//printf("alias: %p\n", ret->op.iValue);
							sprintf_s(buf, 32, "0x%p", ret->op.iValue);
							ret->op.alias = buf;
							//printf("alias2: %s\n", ret->op.alias.c_str());
						}
						else {
							ret->op.op = REGISTER;
							//r1 is register, check if we use alias or formula

							ret->op.alias =ZydisRegisterGetString(inst->operands[1].reg.value);

							//printf("{\n");
							ret->op.r1 = track(inst->operands[1].reg.value, _idx);
							auto t = ret->op.r1;
							//printf("%p / %p / look for mov here! else r1 is going to be reg %i / %i\n",inst->instrAddress,ret->op.r1->rva, inst->operands[1].reg.value, ret->op.r1->op.iValue);
							//maybe set r0 to r1?
							delete ret;
							ret = t; //lets return the r1 trace
							return t;
						}
					}else if (inst->mnemonic == ZYDIS_MNEMONIC_IMUL) {
						ret->op.op = IMUL;
						if (inst->operandCount == 4) {
							ret->op.iValue = inst->operands[2].imm.value.u;
							ret->op.r1 = track(inst->operands[1].reg.value, _idx);
						}
						else {
							auto r0_track = track(inst->operands[0].reg.value, _idx);
							ret->op.r0 = r0_track;

							if (inst->operands[1].type == ZYDIS_OPERAND_TYPE_MEMORY) {
								auto r1_track = track(inst->operands[1].mem.base, _idx);
								ret->op.r1 = r1_track;
							}
							else {
								auto r1_track = track(inst->operands[1].reg.value, _idx);
								ret->op.r1 = r1_track;
							}
							
						}
					}
					else if (inst->mnemonic == ZYDIS_MNEMONIC_XOR) {
						auto r0_track = track(inst->operands[0].reg.value, _idx);
						auto r1_track = track(inst->operands[1].reg.value, _idx);
						ret->op.op = XOR;
						ret->op.r0 = r0_track;
						ret->op.r1 = r1_track;
					}
					else if (inst->mnemonic == ZYDIS_MNEMONIC_SHR) {
						ret->op.op = SHR;
						ret->op.r1 = track(inst->operands[0].reg.value, _idx);
						//printf("tracked %p and got %p / %i\n", ret->rva, ret->op.r1->rva,ret->op.r1->op.iValue);
						//ret->op.r1 = ret->op.r0;
						ret->op.iValue = inst->operands[1].imm.value.u;
					}
					else if (inst->mnemonic == ZYDIS_MNEMONIC_SUB) {
						auto r0_track = track(inst->operands[0].reg.value, _idx);
						auto r1_track = track(inst->operands[1].reg.value, _idx);
						ret->op.op = SUB;
						ret->op.r0 = r0_track;
						ret->op.r1 = r1_track;
					}
					else if (inst->mnemonic == ZYDIS_MNEMONIC_ADD) {
						//printf("%i ftrack Add %p\n", r, inst->instrAddress);
						ret->op.op = ADD;
						auto r0_track = track(inst->operands[0].reg.value, _idx);
						ret->op.r0 = r0_track;
						if (inst->operands[1].reg.value == 0) {
							ret->op.iValue = inst->operands[1].imm.value.u;

						}
						else {
							auto r1_track = track(inst->operands[1].reg.value, _idx);
							ret->op.r1 = r1_track;
						}
					}
					else if (inst->mnemonic == ZYDIS_MNEMONIC_NOT) {
					ret->op.op = NOT;
					ret->op.r1 = track(inst->operands[0].reg.value, _idx);
					ret->op.iValue = inst->operands[1].imm.value.u;
					}
					else if (inst->mnemonic == ZYDIS_MNEMONIC_LEA) {
						ret->op.op = SET;
						//printf("lea reg: %p / %p\n", inst->instrAddress, inst->operands[1].mem.base);
						if (inst->operands[1].mem.base == ZYDIS_REGISTER_RIP) {
							DWORD64 tVal = (inst->length + inst->operands[1].mem.disp.value + inst->instrAddress) - dbg.procBase;
							ret->op.iValue = tVal;
							ret->op.mem_base = true;
						}
						else {
							ret->op.iValue = inst->operands[1].mem.disp.value;
							ret->op.mem_base = true;
						}
					}
					break;
				}
			}
		}
		if (ret->op.r1) {

			if (ret->op.r1->count() > 1) {
				ret->op.formula = ret->op.r1;
				//printf("is formula! %p\n", ret->rva);
			}
			else if (ret->op.r1->op.formula) {
				ret->op.formula = ret->op.r1;
				//printf("!!!!!!!!!!!!!!!! %p r1 has formulA!\n", ret->rva);
			}
		}

		if (ret->rva) {
			for (auto v : vTraces) {
				//check if our x already exists!
				if (ret->rva == v->rva && v->r == r) {
					if (ret->op.formula) v->op.formula = ret->op.formula;
					delete ret;
					//__debugbreak();
					return v;
				}
			}
		}
		else {
			//could not track..
			ret->op.op = REGISTER;
			ret->op.alias = ZydisRegisterGetString(r);
			ret->op.iValue = r;
			/*auto f = register_alias.find(r);
			if (f != register_alias.end()) {
				ret->op.alias = register_alias[r];
			}*/
			//ret->rva = 1;
		}
		//add to list..
		vTraces.push_back(ret);
		return ret;
	}
} regTracker;
std::string find_alias(ZydisRegister r) {
	auto f = regTracker.register_alias.find(r);
	if (f != regTracker.register_alias.end()) {
		return regTracker.register_alias[r];
	}
	return std::string();
}
void ListTrace() {

	ZydisFormatter formatter;
	ZydisFormatterInit(&formatter, ZYDIS_FORMATTER_STYLE_INTEL);

	SendMessage(gui.hListBox2, LB_RESETCONTENT, 0, 0);
	for (DWORD i = 0; i < regTracker.frames.size(); i++) {
		std::string str;
		auto frame = regTracker.frames[i];
		char buf[32];
		sprintf_s(buf, 32, "[0x%08X]: ", frame.rva);
		str += buf;

		auto instruction = frame.get_instruction();
		char buffer[256];
		ZydisFormatterFormatInstruction(&formatter, &instruction, buffer, sizeof(buffer));
		str += buffer;

		SendMessage(gui.hListBox2, LB_ADDSTRING, 0, (LPARAM)str.c_str());
	}
}
void StepOver() {

	CONTEXT c = dbg.GetContext();
	CRegisterFrame f = c;
	regTracker.frames.push_back(f);
	dbg.SingleStep();
	if (bExcept) {
		//skip?
		printf("GOT EXCEPT!\n");
		c.Rip +=f.get_instruction().length;

		dbg.SetContext(&c);
		bExcept = false;
		//return;//
	}
	c = dbg.GetContext();
	char msg[124];
	sprintf_s(msg, 124, "RVA: %p", c.Rip);
	gui.SetStatus(msg);


}



void ShowDisasm() {
	
	ZydisDecoder decoder;
	ZydisDecoderInit(&decoder, ZYDIS_MACHINE_MODE_LONG_64, ZYDIS_ADDRESS_WIDTH_64);

	// Initialize formatter. Only required when you actually plan to do instruction
	// formatting ("disassembling"), like we do here
	ZydisFormatter formatter;
	ZydisFormatterInit(&formatter, ZYDIS_FORMATTER_STYLE_INTEL);

	BYTE bRead[20];

	SendMessage(gui.hListBox, LB_RESETCONTENT, 0, 0);
	if (!dbg.debuggeehProcess) return;
	auto rip = dbg.GetContext().Rip;
	char buf[124];
	for (DWORD i = 0; i < 32; i++) {
		sprintf_s(buf, 124, "[0x%08X]: ", rip - dbg.procBase);
		std::string str = buf;


		dbg.ReadTo(rip, bRead, 20);

		ZydisDecodedInstruction instruction;
		if (ZYDIS_SUCCESS(ZydisDecoderDecodeBuffer(
			&decoder, bRead, 20,
			rip, &instruction))) {
			char buffer[256];
			ZydisFormatterFormatInstruction(&formatter, &instruction, buffer, sizeof(buffer));
			str += buffer;
			rip += instruction.length;
		}

		SendMessage(gui.hListBox, LB_ADDSTRING, 0, (LPARAM)str.c_str());

	}


}
#include <windowsx.h>
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_NOTIFY:

		break;
	case WM_COMMAND:

		switch (LOWORD(wParam))
		{
		case STEP_OVER_BUTTON: {
			char cEdit[32];
			GetWindowTextA(gui.stepEdit, cEdit, 32);
			DWORD c = atoi(cEdit);
			
			for (DWORD i = 0; i < c; i++) {
				StepOver();
			}
			ListTrace();
			break;
		}
		case ADD_ALIAS: {
			DWORD idx = ComboBox_GetCurSel(gui.hAliasCb);
			if (idx != -1) {
				char cAlias[32];
				GetWindowTextA(gui.hAliasEdit, cAlias, 32);
				if (strlen(cAlias) == 0) {//remove
					regTracker.register_alias.erase(ZYDIS_REGISTER_RAX + idx);

				}
				else {
					regTracker.register_alias[ZYDIS_REGISTER_RAX + idx] = cAlias;
				}



				SendMessage(gui.hListBox5, LB_RESETCONTENT, 0, 0);
				for (auto it : regTracker.register_alias) {

					std::string str = std::string(ZydisRegisterGetString(it.first)) + " = " + it.second;// "rcx = not_peb";

					SendMessage(gui.hListBox5, LB_ADDSTRING, 0, (LPARAM)str.c_str());
				}
				//list aliases..

				gui.SetLog("Add alias!");
			}
			break;
		}
		case FILTER_BUTTON: {

			auto c = dbg.GetContext();


			char buf[124];
			GetWindowTextA(gui.ripEdit, buf, 124);
			ULONG_PTR ptr = _strtoui64(buf, NULL, 16);

			c.Rip = dbg.procBase + ptr;
			c.Rcx = 0; //set index register
			dbg.SetContext(&c);
			ShowDisasm();

			gui.SetStatus("rdy now");
			break;
		}
		case IDM_FILE_NEW: {
			printf("Loading %s\n", vLastFiles[0].c_str());
			dbg.InitProcess(vLastFiles[0].c_str());
			dbg.SingleStep();
			bExcept = false;

			auto c = dbg.GetContext();
			char msg[124];
			sprintf_s(msg, 124, "RVA: %p", c.Rip);
			gui.SetStatus(msg);
			gui.SetTitle(vLastFiles[0].c_str());

			//show disasm
			ShowDisasm();

			break;
		}
		case IDM_FILE_OPEN: {
			char filename[MAX_PATH];
			OPENFILENAME ofn;
			ZeroMemory(&filename, sizeof(filename));
			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = NULL;  // If you have a window to center over, put its HANDLE here
			ofn.lpstrFilter = "Exe Files\0*.exe\0Any File\0*.*\0";
			ofn.lpstrFile = filename;
			ofn.nMaxFile = MAX_PATH;
			ofn.lpstrTitle = "Select a File to debug";
			ofn.Flags = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST;

			if (GetOpenFileNameA(&ofn))
			{
				std::cout << "You chose the file \"" << filename << "\"\n";
				//add file to recent cfg

				dbg.InitProcess(filename);
				dbg.SingleStep();
				bExcept = false;

				auto c = dbg.GetContext();
				char msg[124];
				sprintf_s(msg, 124, "RVA: %p", c.Rip);
				gui.SetStatus(msg);
				gui.SetTitle(filename);

				//show disasm
				ShowDisasm();
			}
			break;
		}
		case IDM_FILE_QUIT:
			//reset
			CRegisterFrame::c_idx = 0;
			vTraces.clear();
			regTracker.frames.clear();
			SendMessage(gui.hListBox2, LB_RESETCONTENT, 0, 0);


			//SendMessage(hWnd, WM_CLOSE, 0, 0);
			break;
		case SCAN_LISTBOX2:
			switch (HIWORD(wParam))
			{
			case LBN_SELCHANGE: {
				HWND hwndList = gui.hListBox2;

				// Get selected index.
				int lbItem = (int)SendMessage(hwndList, LB_GETCURSEL, 0, 0);

				char buf[124];
				// Get item data.
				//int i = (int)SendMessage(hwndList, LB_GETTEXT, lbItem, (LPARAM)buf);

				//buf[16] = 0;
				sprintf_s(buf, 124, "%i", lbItem);
				gui.SetStatus(buf);
				//MessageBoxA(0, buf, buf, 0);

				int idx = lbItem;
				auto f = regTracker.frames[idx];
				//track f
				auto i = f.get_instruction();

				auto r0 = i.operands[0].reg.value;
				auto r0_track = regTracker.track(r0, idx+1);
				std::string str = ZydisRegisterGetString(r0);
				SendMessage(gui.hListBox3, LB_RESETCONTENT, 0, 0);
				if (!r0_track ) {
				}
				else {
					sprintf_s(buf, 124, "%i / %s", r0_track->opCount(), ZydisRegisterGetString(r0));
					SendMessage(gui.hListBox3, LB_ADDSTRING, 0, (LPARAM)buf);
					auto p = r0_track;
					while (p && (p->rva || p->op.op == REGISTER)) {

						sprintf_s(buf, 124, "%p / %s", p->rva, p->get_operation().c_str());
						SendMessage(gui.hListBox3, LB_ADDSTRING, 0, (LPARAM)buf);
						p = p->get_prev();
					}
				}

				auto r1 = i.operands[1].reg.value;
				if(i.operands[1].type == ZYDIS_OPERAND_TYPE_MEMORY) r1 = i.operands[1].mem.base;
				auto r1_track = regTracker.track(r1, idx);
				SendMessage(gui.hListBox4, LB_RESETCONTENT, 0, 0);
				if (!r1_track) {
				}
				else {
					sprintf_s(buf, 124, "%i / %s", r1_track->opCount(), ZydisRegisterGetString(r1));
					SendMessage(gui.hListBox4, LB_ADDSTRING, 0, (LPARAM)buf);
					auto p = r1_track;
					while (p && (p->rva || p->op.op == REGISTER)) {

						sprintf_s(buf, 124, "%p / %s", p->rva,p->get_operation().c_str());
						SendMessage(gui.hListBox4, LB_ADDSTRING, 0, (LPARAM)buf);
						p = p->get_prev();
					}
				}
				//print needed formulas too..
				std::vector< CRegisterTrace*> formulas;
				auto formula = r0_track->get_formula(&formulas);

				sprintf_s(buf, 124, "%i sub-formulas\r\n",formulas.size());
				str = buf;
				for (auto it = formulas.rbegin(); it != formulas.rend(); ++it)
				{
					CRegisterTrace* f = *it;
					std::vector< CRegisterTrace*> vFormulas;
					sprintf_s(buf, 124, "auto F_%p = %s;\r\n", f->rva,f->get_formula(&vFormulas).c_str());
					str += buf;
				}
				sprintf_s(buf, 124, "auto _F = %s;\r\n",formula.c_str());
				str += buf;

				gui.SetLog(str.c_str());
			}
			}
			break;
		}
		break;
	case WM_DESTROY:
	{
		SaveCfg();
		ExitProcess(0);
		PostQuitMessage(0);
		return 0;
	}
	break;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}
void LoadCfg() {
	const char* szFile = "asm_dbg.cfg";
	char cVal[256];

	GetPrivateProfileStringA("CFG", "stepTimes", "1", cVal, 124, szFile);
	SetWindowTextA(gui.stepEdit, cVal);
	GetPrivateProfileStringA("CFG", "setRip", "0", cVal, 124, szFile);
	SetWindowTextA(gui.ripEdit, cVal);

	GetPrivateProfileStringA("CFG", "lastFile", "", cVal, 256, szFile);
	printf("last file: %s\n", cVal);

	vLastFiles.push_back(cVal);
}
void SaveCfg() {
	//..
	const char* szFile = "asm_dbg.cfg";
	char cVal[64];
	GetWindowTextA(gui.stepEdit, cVal, 64);
	WritePrivateProfileStringA("CFG", "stepTimes", cVal, szFile);
	GetWindowTextA(gui.ripEdit, cVal, 64);
	WritePrivateProfileStringA("CFG", "setRip", cVal, szFile);

	//save last files..
	if (!dbg.filename.empty()) {
		WritePrivateProfileStringA("CFG", "lastFile", dbg.filename.c_str(), szFile);
	}
}
int main() {
	gui.Init();
	LoadCfg();
	//dbg.InitProcess("C:\\Games\\Call of Duty Modern Warfare\\ModernWarfare_dump 22-07.exe");
	//dbg.SingleStep();

	auto c = dbg.GetContext();
	//c.Rip = dbg.procBase+ 0x14343DD;
	//dbg.SetContext(&c);

	char msg[124];
	sprintf_s(msg, 124, "RVA: %p", c.Rip);
	gui.SetStatus(msg);

	/*for (DWORD i = 0; i < 32; i++) {

		StepOver();
	}*/

	//show disasm
	ShowDisasm();

	gui.Run();
	return 0;

}

// the entry point for any Windows program
int WINAPI WinMain(HINSTANCE _hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow) {
	hInstance = _hInstance;

	return 0;
}


void TestOutput() {
	DWORD rax = 0;
	DWORD rbx = 0;
	DWORD rcx = 0;
	DWORD BASE_MEM = 0;
	DWORD BSWAP = 0;
	auto F_000000000143476E = (rax >> 0x1E);
	auto F_0000000001434772 = ((rax) ^ F_000000000143476E);
	auto F_0000000001434778 = (F_0000000001434772 >> 0x3C);
	auto F_0000000001434736 = (~rbx);
	auto F_0000000001434746 = (((F_0000000001434772 ^ F_0000000001434778) ^ F_0000000001434736) ^ (BASE_MEM + 0x4244D8BD));
	auto F_00000000014345A0 = ((0x36CF5A33B9962C9B) + F_0000000001434746);
	auto _F = ((((BSWAP + 0xD) * F_00000000014345A0) * 0x6032E6DF0F6B4331) - 0x5C38D3E559DDA29B);
}
