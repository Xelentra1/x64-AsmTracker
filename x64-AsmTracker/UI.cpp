#include <Windows.h>
#include <CommCtrl.h>

#pragma comment(lib,"Dbghelp.lib")
#pragma comment(lib,"Comctl32.lib")
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include <vector>
#include "Dbg.hpp"
#include "Lua.hpp"

HINSTANCE hInstance;
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK ScriptWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
#define SCREEN_WIDTH  1000
#define SCREEN_HEIGHT 832

//options
bool bShowComments = true;

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
	IDM_FILE_QUIT,
	//script wnd
	EXECUTE_SCRIPT,
	SCRIPT_BOX,
};
#include "UI.h"
CScriptGUI* scriptGui = NULL;

void MakeGroupBox(HWND hWnd, LPCWSTR szLabel, DWORD x, DWORD y, DWORD w, DWORD h) {
	HWND hGrpButtons = CreateWindowExW(WS_EX_WINDOWEDGE,
		L"BUTTON",szLabel,WS_VISIBLE | WS_CHILD | BS_GROUPBOX,
		x - 14, y - 16,	w + 20, h + 28,
		hWnd,(HMENU)4,hInstance, NULL);
	SendMessage(hGrpButtons, WM_SETFONT, WPARAM(GetStockObject(DEFAULT_GUI_FONT)), TRUE);
}
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
	HWND hListView;
	bool bFinish = false;
	HFONT fConsolas;
	bool bRunLua = false;
	void Init() {
		fConsolas = CreateFont(14, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Consolas");
		INITCOMMONCONTROLSEX icex;           // Structure for control initialization.
		icex.dwICC = ICC_LISTVIEW_CLASSES;
		InitCommonControlsEx(&icex);
		
		scriptGui = new CScriptGUI;
		scriptGui->Init();

		const char* wndClass = "main_wndclass";
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


		MakeGroupBox(hWnd, L"DISASM",18, 18, 260, 360);

		hListBox = CreateWindowEx(0, WC_LISTBOXA, NULL,
			WS_CHILD | WS_VISIBLE | WS_VSCROLL | LBS_NOTIFY,
			18, 18, 260, 360,
			hWnd, (HMENU)SCAN_LISTBOX, hInstance, NULL);
		SendMessage(hListBox, WM_SETFONT, wFont, TRUE);


		MakeGroupBox(hWnd, L"DEBUG FRAMES", 308, 18, 460, 360);

		hListView = CreateWindowEx(0, WC_LISTVIEWA, NULL,
			WS_CHILD | WS_VISIBLE | WS_VSCROLL | LVS_REPORT | LVS_OWNERDATA,
			308, 18, 460, 360,
			hWnd, (HMENU)SCAN_LISTVIEW, hInstance, NULL);
		SendMessage(hListView, WM_SETFONT, WPARAM(GetStockObject(DEFAULT_GUI_FONT)), TRUE);
		SendMessage(hListView, LVM_SETEXTENDEDLISTVIEWSTYLE,
			0, LVS_EX_FULLROWSELECT); // Set style
		LVCOLUMNA LvCol;
		// Here we put the info on the Coulom headers
		// this is not data, only name of each header we like
		memset(&LvCol, 0, sizeof(LvCol));                  // Zero Members

		LvCol.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;    // Type of mask
		LvCol.cx = 0x28;                                   // width between each coloum
		LvCol.pszText = (LPSTR)"Offset";                            // First Header Text
		LvCol.cx = 80;                                   // width of column
														   // Inserting Couloms as much as we want
		SendMessage(hListView, LVM_INSERTCOLUMN, 0, (LPARAM)&LvCol); // Insert/Show the coloum
		LvCol.pszText = (LPSTR)"Name";                            // Next coloum
		LvCol.cx = 200;                                   // width of column
		SendMessage(hListView, LVM_INSERTCOLUMN, 1, (LPARAM)&LvCol); // ...
		LvCol.pszText = (LPSTR)"Value";                            //
		LvCol.cx = 180;                                   // width of column
		SendMessage(hListView, LVM_INSERTCOLUMN, 2, (LPARAM)&LvCol); //
																	 //draw listview and tableview

																	 //edit options


		MakeGroupBox(hWnd, L"R0", 308, 406, 260, 160);

		hListBox3 = CreateWindowEx(0, WC_LISTBOXA, NULL,
			WS_CHILD | WS_VISIBLE | WS_VSCROLL | LBS_NOTIFY,
			308, 406, 260, 160,
			hWnd, (HMENU)SCAN_LISTBOX3, hInstance, NULL);
		SendMessage(hListBox3, WM_SETFONT, wFont, TRUE);


		MakeGroupBox(hWnd, L"R1", 598, 406, 260, 160);

		hListBox4 = CreateWindowEx(0, WC_LISTBOXA, NULL,
			WS_CHILD | WS_VISIBLE | WS_VSCROLL | LBS_NOTIFY,
			598, 406, 260, 160,
			hWnd, (HMENU)SCAN_LISTBOX4, hInstance, NULL);
		SendMessage(hListBox4, WM_SETFONT, wFont, TRUE);

		MakeGroupBox(hWnd, L"FORMULA", 308, 596, 620, 160);

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

		SendMessage(hwndEdit, WM_SETFONT, WPARAM(fConsolas), TRUE);


		MakeGroupBox(hWnd, L"ALIAS", 18, 596, 260, 140);


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


		MoveWindow(hWnd, GetSystemMetrics(SM_CXSCREEN)- SCREEN_WIDTH-80,380,SCREEN_WIDTH, SCREEN_HEIGHT, false);

		ShowWindow(hWnd, SW_SHOWNORMAL);
	}
	void Run() {
		MSG msg;
		while (!bFinish) {
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
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

	void CScriptGUI::Init() {

		const char* wndClass = "script_wndclass";
		WNDCLASSEXA wc;
		ZeroMemory(&wc, sizeof(WNDCLASSEX));
		wc.cbSize = sizeof(WNDCLASSEX);
		//wc.style = CS_DBLCLKS | CS_GLOBALCLASS;// CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
		wc.lpfnWndProc = ScriptWindowProc;
		wc.lpszClassName = wndClass;
		wc.hInstance = hInstance;//GetModuleHandle(nullptr);
		//wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)(COLOR_WINDOW);//(HBRUSH)GetStockObject(BLACK_BRUSH);//reinterpret_cast<HBRUSH>(COLOR_WINDOW);
		//wc.lpszClassName = wndClass;
		RegisterClassExA(&wc);
		unsigned int dwStyle = (WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX);
		hWnd = CreateWindowExA(NULL, wndClass, "x64-ASM Tracker - Script Engine", dwStyle, 300, 300, 500,600, nullptr, nullptr, GetModuleHandle(nullptr), nullptr);

		WPARAM wFont = WPARAM(GetStockObject(DEFAULT_GUI_FONT));
		SetParent(hWnd, gui.hWnd);

		MakeGroupBox(hWnd, L"SCRIPT", 28, 28, 740, 320);
		hScriptEdit = CreateWindowExA(
			0, "EDIT",   // predefined class 
			NULL,         // no window title 
			WS_CHILD | WS_VISIBLE | WS_VSCROLL |
			ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL,
			28, 28, 620, 320,   // set size in WM_SIZE message 
			hWnd,         // parent window 
			(HMENU)LOG_BOX,   // edit control ID 
			hInstance,
			NULL);        // pointer not needed 

		SetWindowText(hScriptEdit,
			"if(not FileLoaded()) then LoadPreviousFile()\r\n"
			"else ResetDbg() end\r\n"
			"local r = GetBase()+0x1434492\r\n"
			/*"local r = DoScan('0F B6 4C 24 40 48 C1 C9 0C 83 E1 0F 48 83 F9 0E')\r\n"
			"local iSkip = 0\r\n"
			"Decode(GetBase()+r,function(rip,it)\r\n"
			"\tiSkip=iSkip+1 r = rip return iSkip<6\r\n"
			"end)\r\n"
			"Log(string.format('Scan Result: %x',r))\r\n"*/
			"SetRva(r-GetBase())\r\n"
			"local tc = GetTickCount()\r\n"
			"SetAlias(RAX,'ret')\r\n"
			"StepOver(220)\r\n"
			"Log('tc: '..(GetTickCount()-tc)/1000)\r\n"
			"local c = GetContext()\r\n"
			"Log(string.format('Hi! %x',c.rip))\r\n"
			"tc = GetTickCount()\r\n"
			"local t = Track(RAX)\r\n"
			"Log('tc2: '..(GetTickCount()-tc)/1000)\r\n"
			"Log(string.format('RAX Track! %x',t.rva))\r\n"
			"Log('Dump {') Log(t:dump()) Log('}')"
		
		);
		auto hFont = gui.fConsolas;
		SendMessage(hScriptEdit, WM_SETFONT, WPARAM(hFont), TRUE);

		HWND hSearch = CreateWindowEx(0, WC_BUTTONA, NULL,
			WS_CHILD | WS_VISIBLE,
			658, 32, 82, 22,
			hWnd, (HMENU)EXECUTE_SCRIPT, hInstance, NULL);
		SetWindowText(hSearch, "EXECUTE");
		SendMessage(hSearch, WM_SETFONT, wFont, TRUE);

		hScriptBox = CreateWindowEx(0, WC_LISTBOXA, NULL,
			WS_CHILD | WS_VISIBLE | WS_VSCROLL | LBS_NOTIFY,
			648, 68, 102, 140,
			hWnd, (HMENU)SCRIPT_BOX, hInstance, NULL);
		SendMessage(hScriptBox, WM_SETFONT, wFont, TRUE);
		SendMessage(hScriptBox, LB_ADDSTRING, 0, (LPARAM)"New");
		SendMessage(hScriptBox, LB_SETCURSEL, 0, 0);


		MakeGroupBox(hWnd, L"LOG", 28, 388, 740, 160);
		hLogEdit = CreateWindowExA(
			0, "EDIT",   // predefined class 
			NULL,         // no window title 
			WS_CHILD | WS_VISIBLE | WS_VSCROLL |
			ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL,
			28, 388, 740, 160,   // set size in WM_SIZE message 
			hWnd,         // parent window 
			(HMENU)LOG_BOX,   // edit control ID 
			hInstance,
			NULL);        // pointer not needed 

		SendMessage(hLogEdit, WM_SETFONT, WPARAM(hFont), TRUE);

		MoveWindow(hWnd, GetSystemMetrics(SM_CXSCREEN) - SCREEN_WIDTH - 280, 40, 800, 600, false);

		ShowWindow(hWnd, SW_SHOWNORMAL);
	}

DWORD CRegisterFrame::c_idx = 0;

template <class T>
bool VectorContains(std::vector<T> vec,T cmp) {
	bool bListed = false;
	for (DWORD k = 0; !bListed && k < vec.size(); k++) {
		if (cmp == vec[k]) bListed = true;
	}
	return bListed;
}
std::string find_alias(ZydisRegister r);

DWORD CRegisterTrace::opCount() {
	auto p = this;
	DWORD c = 0;
	while (p) {
		if (p->rva)c++;
		p = p->get_prev();
	}
	return c;
}
	DWORD CRegisterTrace::count() {
		//if ( op.op == ALIAS) return 0;
		DWORD i = 1;
		if (op.r1) i += op.r1->count();
		return i;
	}
	CRegisterTrace* CRegisterTrace::get_prev() {
		return op.r0;
	}
	std::string CRegisterTrace::get_operation() {
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
	DWORD CRegisterTrace::get_formulas(std::vector< CRegisterTrace*>* vFormulas) {
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
	std::string CRegisterTrace::get_formula(std::vector< CRegisterTrace*>* vFormulas) {
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

std::vector< CRegisterTrace*> vTraces;
#include <unordered_map>
	CRegisterTrace* CRegisterTracker::track(ZydisRegister r, DWORD idx) {
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
			else if (it == frames.rbegin()) continue; //skip first.. //loop till cur frame..


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
					} else if (inst->mnemonic == ZYDIS_MNEMONIC_MOVSXD) {
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
								//ret->op.r1->op.op = BSWAP;//hardcode to bswap..
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
					else {
						//unk?
						ret->rva = 0;
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
	CRegisterTracker* regTracker;

std::string find_alias(ZydisRegister r) {
	auto f = regTracker->register_alias.find(r);
	if (f != regTracker->register_alias.end()) {
		return regTracker->register_alias[r];
	}
	return std::string();
}

void ListTrace() {
	SendMessage(gui.hListView, LVM_SETITEMCOUNT, regTracker->frames.size()-1, 0);
}
void StepOver() {

	CONTEXT c = dbg.GetContext();
	CRegisterFrame f = c;
	regTracker->frames.push_back(f);
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

std::string DumpFormula(CRegisterTrace* r0_track) {

	std::vector< CRegisterTrace*> formulas;
	auto formula = r0_track->get_formula(&formulas);

	char buf[256];
	sprintf_s(buf, 124, "%i sub-formulas\r\n", formulas.size());
	std::string str = buf;
	for (auto it = formulas.rbegin(); it != formulas.rend(); ++it)
	{
		CRegisterTrace* f = *it;
		std::vector< CRegisterTrace*> vFormulas;
		sprintf_s(buf, 256, "auto F_%p = %s;\r\n", f->rva, f->get_formula(&vFormulas).c_str());
		str += buf;
	}
	sprintf_s(buf, 124, "auto _F = %s;\r\n", formula.c_str());
	str += buf;
	return str;
}

void ShowTrace(int idx) {
	if (idx < 0) return;
	char buf[256];
	// Get item data.
	//int i = (int)SendMessage(hwndList, LB_GETTEXT, lbItem, (LPARAM)buf);

	//buf[16] = 0;
	sprintf_s(buf, 124, "%i", idx);
	gui.SetStatus(buf);
	//MessageBoxA(0, buf, buf, 0);

	auto f = regTracker->frames[idx];
	//track f
	auto i = f.get_instruction();

	auto r0 = i.operands[0].reg.value;
	auto r0_track = regTracker->track(r0, idx + 1);
	std::string str = ZydisRegisterGetString(r0);
	SendMessage(gui.hListBox3, LB_RESETCONTENT, 0, 0);
	if (!r0_track) {
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
	if (i.operands[1].type == ZYDIS_OPERAND_TYPE_MEMORY) r1 = i.operands[1].mem.base;
	auto r1_track = regTracker->track(r1, idx);
	SendMessage(gui.hListBox4, LB_RESETCONTENT, 0, 0);
	if (!r1_track) {
	}
	else {
		sprintf_s(buf, 124, "%i / %s", r1_track->opCount(), ZydisRegisterGetString(r1));
		SendMessage(gui.hListBox4, LB_ADDSTRING, 0, (LPARAM)buf);
		auto p = r1_track;
		while (p && (p->rva || p->op.op == REGISTER)) {

			sprintf_s(buf, 124, "%p / %s", p->rva, p->get_operation().c_str());
			SendMessage(gui.hListBox4, LB_ADDSTRING, 0, (LPARAM)buf);
			p = p->get_prev();
		}
	}
	//print needed formulas too..
	str = DumpFormula(r0_track);

	gui.SetLog(str.c_str());
}
#include <windowsx.h>


void LoadPreviousFile() {
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
}

LRESULT CALLBACK ScriptWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:

		switch (LOWORD(wParam))
		{
		case EXECUTE_SCRIPT: {
			gui.bRunLua = true;
			SendMessage(gui.hWnd, WM_PAINT, 0, 0);
			break;
		}
		}
		break;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}
void ResetDbg() {
	//reset
	CRegisterFrame::c_idx = 0;
	vTraces.clear();
	regTracker->frames.clear();
	SendMessage(gui.hListBox2, LB_RESETCONTENT, 0, 0);
}

void SetAlias(DWORD reg,std::string str) {
	if (str.empty()) {//remove
		regTracker->register_alias.erase(reg);

	}
	else {
		regTracker->register_alias[reg] = str;
	}

	SendMessage(gui.hListBox5, LB_RESETCONTENT, 0, 0);
	for (auto it : regTracker->register_alias) {

		std::string str = std::string(ZydisRegisterGetString(it.first)) + " = " + it.second;// "rcx = not_peb";

		SendMessage(gui.hListBox5, LB_ADDSTRING, 0, (LPARAM)str.c_str());
	}
	//list aliases..

	//reset traces comments

	for (DWORD i = 0; i < regTracker->frames.size(); i++) {
		auto f = &regTracker->frames[i];
		f->comment.clear();
		f->iComment = 0;
	}
	ListTrace();
}
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	if (gui.bRunLua) {
		char szScript[1024 * 10];
		GetWindowTextA(scriptGui->hScriptEdit, szScript, 1024 * 10);
		gui.bRunLua = false;

		ExecuteLua(szScript);
	}
	switch (message)
	{
	case WM_NOTIFY:
		if ((((LPNMHDR)lParam)->hwndFrom) == gui.hListView)
		{
			switch (((LPNMHDR)lParam)->code)
			{
			case LVN_GETDISPINFO:
			{
				NMLVDISPINFO* plvdi = (NMLVDISPINFO*)lParam;
				DWORD idx = plvdi->item.iItem;
				//printf("disp info! %i\n",idx);
				auto &f = regTracker->frames[idx];
				if (plvdi->item.mask & LVIF_TEXT)
				{
					DWORD iSub = plvdi->item.iSubItem;
					if (iSub == 0) {
						//address.
						char msg[124];
						sprintf_s(plvdi->item.pszText, 124, "0x%08X", f.rva);
					}
					else if (iSub == 1) {
						strcpy_s(plvdi->item.pszText, 64,f.szInstruction);
					}
					else {
						//comment
						if (f.iComment == 0 && bShowComments) {//look for comment..
							//printf("gen comment..\n");
							f.iComment = 3; //means we scanned

							auto instruction = f.get_instruction();
							//build comment here..
							std::string comment;
							auto t = regTracker->track(instruction.operands[0].reg.value, idx + 1);
							if (t && regTracker->register_alias.find(instruction.operands[0].reg.value) != regTracker->register_alias.end()) {
								f.iComment = 1;
								char cComment[256];
								std::vector< CRegisterTrace*> vFormulas;
								sprintf_s(cComment, 256, "%s = %s %s //%s", ZydisRegisterGetString(instruction.operands[0].reg.value), ZydisRegisterGetString(instruction.operands[0].reg.value), t->get_operation().c_str(), t->op.r1->get_formula(&vFormulas).c_str());
								f.comment = cComment;
							}
							else {
								f.iComment = 2;
								auto t = regTracker->track(instruction.operands[1].reg.value, idx+ 1);
								if (t &&

									(regTracker->register_alias.find(instruction.operands[1].reg.value) != regTracker->register_alias.end())
									) {
									char cComment[256];
									sprintf_s(cComment, 256, "%s = %s", ZydisRegisterGetString(instruction.operands[0].reg.value), t->get_operation().c_str());
									f.comment = cComment;
								}
							}
						}

						if ((f.iComment == 1 || f.iComment == 2) && !f.comment.empty()) {
							strcpy_s(plvdi->item.pszText, 124, f.comment.c_str());
						}

					}
				}

				break;
			}
			case NM_CUSTOMDRAW:
			{
				LPNMLVCUSTOMDRAW  lplvcd = (LPNMLVCUSTOMDRAW)lParam;
				switch (lplvcd->nmcd.dwDrawStage) {
				case CDDS_PREPAINT:
					return CDRF_NOTIFYITEMDRAW;
					break;
				case CDDS_SUBITEM | CDDS_ITEMPREPAINT:
				case CDDS_ITEMPREPAINT: {
					if (((int)lplvcd->nmcd.dwItemSpec % 2) == 0) {
						lplvcd->clrText = RGB(0, 0, 0);
						lplvcd->clrTextBk = RGB(222, 222, 222);
					}
					else {
						lplvcd->clrText = RGB(0, 0, 0);
						lplvcd->clrTextBk = RGB(255, 255, 255);
					}
					if (lplvcd->iSubItem == 2) {
						//printf("disp info! %i\n",idx);
						auto f = &regTracker->frames[lplvcd->nmcd.dwItemSpec];
						
						if (f&&!f->comment.empty()) {
							if (f->iComment == 1)
								lplvcd->clrText = RGB(235, 61, 52); //red
							else
								lplvcd->clrText = RGB(52, 152, 235); //blue
						}
					}

					return CDRF_NOTIFYSUBITEMDRAW;
					break;
				}
				}


				OutputDebugStringA("DRAW\n");
				return TRUE;
			}

			case NM_CLICK:
			{
				char msg[124];
				int ItemIndex = SendMessage(gui.hListView, LVM_GETNEXTITEM, -1, LVNI_SELECTED);
				printf("self: %i\n", ItemIndex);
				ShowTrace(ItemIndex);
				break;
			}
		}
		}
		break;
	case WM_COMMAND:

		switch (LOWORD(wParam))
		{
		case STEP_OVER_BUTTON: {
			char cEdit[32];
			GetWindowTextA(gui.stepEdit, cEdit, 32);
			DWORD c = atoi(cEdit);
			
			auto tc = GetTickCount();
			for (DWORD i = 0; i < c; i++) {
				StepOver();
			}
			ListTrace();
			printf("tc: %f\n", (GetTickCount() - tc) / 1000.f);
			break;
		}
		case ADD_ALIAS: {
			DWORD idx = ComboBox_GetCurSel(gui.hAliasCb);
			if (idx != -1) {
				char cAlias[32];
				GetWindowTextA(gui.hAliasEdit, cAlias, 32);
				SetAlias(ZYDIS_REGISTER_RAX + idx,cAlias);
			}
			break;
		}
		case FILTER_BUTTON: {

			auto c = dbg.GetContext();


			char buf[124];
			GetWindowTextA(gui.ripEdit, buf, 124);
			ULONG_PTR ptr = _strtoui64(buf, NULL, 16);

			c.Rip = dbg.procBase + ptr;
			dbg.SetContext(&c);
			ShowDisasm();

			gui.SetStatus("rdy now");
			break;
		}
		case IDM_FILE_NEW: {
			LoadPreviousFile();

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
			ResetDbg();


			//SendMessage(hWnd, WM_CLOSE, 0, 0);
			break;
		case SCAN_LISTBOX2:
			switch (HIWORD(wParam))
			{
			case LBN_SELCHANGE: {
				HWND hwndList = gui.hListBox2;

				// Get selected index.
				int lbItem = (int)SendMessage(hwndList, LB_GETCURSEL, 0, 0);

				ShowTrace(lbItem);
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
	regTracker = new CRegisterTracker;
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
	DWORD rip = 0;
	DWORD alias = 0;
	DWORD peb = 0;
	DWORD not_peb = 0;
	DWORD BASE_MEM = 0;
	DWORD BSWAP = 0;
	auto F_0000000001441212 = (BSWAP);
	auto F_0000000001441215 = ((alias)*F_0000000001441212);
	auto F_00000000014412BB = (F_0000000001441215 >> 0x1E);
	auto F_00000000014412BF = ((F_0000000001441215 ^ F_00000000014412BB));
	auto F_00000000014412C5 = (F_00000000014412BF >> 0x3C);
	auto F_00000000014413B4 = (~peb);
	auto F_00000000014413C1 = ((((((F_00000000014412BF ^ F_00000000014412C5) + 0x707C09951ACCCD60) + peb) * 0x1BE9057640A87F4F) + F_00000000014413B4) + (BASE_MEM + 0x5E2D6625));
	auto F_00000000014413C7 = (F_00000000014413C1 >> 0x0A);
	auto F_00000000014413CB = ((F_00000000014413C1 ^ F_00000000014413C7));
	auto F_0000000001441222 = (F_00000000014413CB >> 0x14);
	auto F_0000000001441226 = ((F_00000000014413CB ^ F_0000000001441222));
	auto F_000000000144146C = (F_0000000001441226 >> 0x28);
	auto F_000000000144147A = (~(BASE_MEM + 0x6E874B86));
	auto _F = (((F_0000000001441226 ^ F_000000000144146C) - peb) + F_000000000144147A);
}
