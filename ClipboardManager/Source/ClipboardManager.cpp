//	each clipboard manager is compiled from 2 files: ClipboardManager.cpp and ClipboardManager<X>.cpp (unity build)
//	e.g. ClipboardManager.cpp and ClipboardManagerDual.cpp -> ClipboardManagerDual.exe
//	ClipboardManager<X>.cpp needs to implement functions mentioned in ClipboardManager.hpp

//	TODO
//	This file needs to implement wWinMain and hooks
//	main should
//		- get my PID
//		- save PID to file Data/clipboard_manager.bin 
//		- (can we open FILE for reading and keep it for later? Note that file should be deletable by others)
//		- install low level keyboard hook
//		- install clipboard update listener
//		- MSG loop
//			- loop should check Data/clipboard_manager.bin 
//			- if PID's differ (or if file does not exist) program should exit
//	low level keyboard hook
//		- update key_state
//		- if Ctrl (Shift) C/X/V, call onTargetKeys
//			- not if any other modifier key (Alt, Win)
//			- not if repeated (only 1st key Down event)
//	clipboard update listener
//		- should call onClipboardUpdate

#include <windows.h>
#include <cstdio>
#include "./ClipboardManager.hpp"
#include "./CppPatterns.hpp"

static HWND g_hwnd = nullptr;
static DWORD g_pid = 0;

constexpr char const* FILENAME = "Data/clipboard_manager.bin";

static bool checkPidFile() {
	FILE* f = fopen(FILENAME, "rb");
	if (!f) {
		LOG("Could not open log file (rb) %s", FILENAME);
		return false;
	}
	defer { fclose(f); };
	
	DWORD stored_pid = 0;
	size_t read = fread(&stored_pid, sizeof(DWORD), 1, f);
	if (read != 1) {
		return false;
	}
	
	return stored_pid == g_pid;
}

static bool savePidFile() {
	FILE* f = fopen(FILENAME, "wb");
	if (!f) {
		LOG("Could not open log file (wb) %s", FILENAME);
		return false;
	}
	defer { fclose(f); };
	
	if (fwrite(&g_pid, sizeof(DWORD), 1, f) != 1) {
		LOG("Could not write PID %lu into %s", g_pid, FILENAME);
		return false;
	}
	return true;
}

static KeyHookResult keyHandler(int nCode, WPARAM wParam, LPARAM lParam) {
	if (nCode != HC_ACTION) { return NEXT_HOOK; }
	
	KBDLLHOOKSTRUCT* kbd = (KBDLLHOOKSTRUCT*)lParam;
	if (kbd->flags & LLKHF_INJECTED) { return NEXT_HOOK; }
	
	DWORD vk = kbd->vkCode;
	bool is_down = (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN);
	bool is_up = (wParam == WM_KEYUP || wParam == WM_SYSKEYUP);
	bool repeated = !is_down && !is_up;
	
	if (repeated) { return NEXT_HOOK; }
	
	repeated = true;
	switch (vk) {
		case VK_CONTROL : if (key_state.ctrl   != is_down) { repeated = false; key_state.ctrl   = is_down; } break;
		case VK_LCONTROL: if (key_state.ctrll  != is_down) { repeated = false; key_state.ctrll  = is_down; } break;
		case VK_RCONTROL: if (key_state.ctrlr  != is_down) { repeated = false; key_state.ctrlr  = is_down; } break;
		case VK_SHIFT   : if (key_state.shift  != is_down) { repeated = false; key_state.shift  = is_down; } break;
		case VK_LSHIFT  : if (key_state.shiftl != is_down) { repeated = false; key_state.shiftl = is_down; } break;
		case VK_RSHIFT  : if (key_state.shiftr != is_down) { repeated = false; key_state.shiftr = is_down; } break;
		case 'C'        : if (key_state.c      != is_down) { repeated = false; key_state.c      = is_down; } break;
		case 'X'        : if (key_state.x      != is_down) { repeated = false; key_state.x      = is_down; } break;
		case 'V'        : if (key_state.v      != is_down) { repeated = false; key_state.v      = is_down; } break;
		default: return NEXT_HOOK;
	}
	if (repeated) { return NEXT_HOOK; }
	
	if (is_down) {
		bool alt = (GetAsyncKeyState(VK_LMENU) | GetAsyncKeyState(VK_RMENU) | GetAsyncKeyState(VK_MENU)) & 0x8000;
		bool win = (GetAsyncKeyState(VK_LWIN ) | GetAsyncKeyState(VK_RWIN )) & 0x8000;
		bool is_target = vk == 'C' || vk == 'X' || vk == 'V';
		
		if (alt) {
			bool shift = (GetAsyncKeyState(VK_LSHIFT) | GetAsyncKeyState(VK_RSHIFT) | GetAsyncKeyState(VK_SHIFT)) & 0x8000;
			if (shift && is_target) {
				return onAltTarget(vk);
			}
			return NEXT_HOOK;
		}
		if (win) { return NEXT_HOOK; }
		
		if (key_state.ctrla() && is_target) {
			if (!checkPidFile()) {
				//	this callback does not go through main loop, therefore...
				LOG("Sending quit");
				PostQuitMessage(0);
				return NEXT_HOOK;
			}
			LOG("Handle key");
			return onTargetKeys(vk, key_state.shifta(), nCode, wParam, lParam);
		}
	}
	return NEXT_HOOK;
}

static LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
	auto result = keyHandler(nCode, wParam, lParam);
	if (result.ok) { return result.data; }
	return CallNextHookEx(g_keyboard_hook, nCode, wParam, lParam);
}

static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (msg == WM_CLIPBOARDUPDATE) {
		//	No need here because this goes through main loop
		//	if (!checkPidFile()) {
		//		LOG("Sending quit");
		//		PostQuitMessage(0);
		//	} else {
		LOG("Handle clipboard update");
		onClipboardUpdate();
		return 0;
	}
	return DefWindowProcW(hwnd, msg, wParam, lParam);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR, int) {
	g_pid = GetCurrentProcessId();
	
	if (!savePidFile()) {
		LOG("Closing because of error e41878e4-602a-4d56-8727-cf1cd2b380fd");
		return 1;
	}
	
	WNDCLASSW wc = {};
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = L"ClipboardManagerClass";
	RegisterClassW(&wc);
	
	g_hwnd = CreateWindowExW(
		0,
		L"ClipboardManagerClass",
		L"ClipboardManager",
		0,
		0, 0, 0, 0,
		HWND_MESSAGE,
		nullptr,
		hInstance,
		nullptr
	);
	
	if (!g_hwnd) {
		LOG("Could not open window");
		return 1;
	}
	defer { 
		if (g_hwnd) {
			DestroyWindow(g_hwnd); 
		}
	};
	
	if (!AddClipboardFormatListener(g_hwnd)) {
		LOG("Could not AddClipboardFormatListener");
		return 1;
	}
	defer { RemoveClipboardFormatListener(g_hwnd); };
	
	g_keyboard_hook = SetWindowsHookExW(
		WH_KEYBOARD_LL,
		LowLevelKeyboardProc,
		hInstance,
		0
	);
	
	if (!g_keyboard_hook) {
		LOG("Could not keyboard hook");
		return 1;
	}
	defer { 
		if (g_keyboard_hook) {
			UnhookWindowsHookEx(g_keyboard_hook); 
		}
	};
	
	if (!init()) {
		LOG("init failed");
		return 1;
	}
	
	int exit_code = 0;
	while (true) {
		MSG msg;
		BOOL ret = GetMessage(&msg, nullptr, 0, 0);
		if (ret == 0 || ret == -1) { 
			exit_code = msg.wParam; 
			if (exit_code == 0 && ret == -1) { exit_code = 1; }
			LOG("closing because of return %d 4b8d5199-42ee-47e8-ab14-9468cdff013b", (int) ret); 
			break;
		}
		if (!checkPidFile()) { 
			LOG("Closing because of pid mismatch 36b7f9a2-da7c-4463-aea5-5009850c2ec2"); 
			break;
		}
		else { LOG("Pid ok"); }
		DispatchMessage(&msg);
	}
	
	LOG("Exiting")
	return exit_code;
}