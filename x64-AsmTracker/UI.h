#pragma once


class CScriptGUI {
public:
	HWND hWnd;
	HWND hLogEdit;
	HWND hScriptEdit;
	HWND hScriptBox;
	void Init();
};
extern CScriptGUI* scriptGui;