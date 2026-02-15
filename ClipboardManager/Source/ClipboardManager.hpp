#pragma once

#include <windows.h>

#include "./Utils.hpp"

//	functions to be implemented by ClipboardManager<X>.cpp

using KeyHookResult = Result<LRESULT>;
constexpr KeyHookResult NEXT_HOOK = KeyHookResult :: MY_NO_DATA;
constexpr KeyHookResult BLOCK     = 1;
bool init();
void onClipboardUpdate();
KeyHookResult onTargetKeys(DWORD vk_code, bool shift, int nCode, WPARAM wParam, LPARAM lParam);
KeyHookResult onAltTarget (DWORD vk_code);

//	global state that may be used (but not modified) in ClipboardManager<X>.cpp
struct {
	bool  ctrl  = false;
	bool  ctrll = false;
	bool  ctrlr = false;
	bool shift  = false;
	bool shiftl = false;
	bool shiftr = false;
	bool c = false;
	bool x = false;
	bool v = false;
	
	bool  ctrla() const { return  ctrl ||  ctrll ||  ctrlr; }
	bool shifta() const { return shift || shiftl || shiftr; }
} key_state;

static HHOOK g_keyboard_hook = nullptr;
