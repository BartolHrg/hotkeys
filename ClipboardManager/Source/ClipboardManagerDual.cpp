#include "./ClipboardManager.hpp"
#include "./ClipboardUtils.cpp"
#include "./CppPatterns.hpp"

static Clipboard :: AllData clipboard_a;
static Clipboard :: AllData clipboard_b;
static bool current_is_a = true;
static bool onupdate_enabled = true;
static bool should_shift_down = false;

bool init() {
	auto data = Clipboard :: get();
	if (!data.ok) { return false; }
	clipboard_a = data.data; // copy
	clipboard_b = data.data; // copy
	return true;
}

#define _shiftDownWithAsync(state, vk) _shiftDown((state) && (GetAsyncKeyState(vk) & 0x8000), (vk))
void _shiftDown(bool should, DWORD vk) {
	if (!should) { return; }
	INPUT input = {};
	input.type = INPUT_KEYBOARD;
	input.ki.wVk = vk;
	input.ki.dwFlags = 0;
	SendInput(1, &input, sizeof(INPUT));
}
void _shiftUp(bool should, DWORD vk) {
	if (!should) { return; }
	INPUT input = {};
	input.type = INPUT_KEYBOARD;
	input.ki.wVk = vk;
	input.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &input, sizeof(INPUT));
}
#define shiftDownWithAsync() _shiftDownWithAsync(key_state.shift , VK_SHIFT ), _shiftDownWithAsync(key_state.shiftl, VK_LSHIFT), _shiftDownWithAsync(key_state.shiftr, VK_RSHIFT)
#define shiftDown()          _shiftDown         (key_state.shift , VK_SHIFT ), _shiftDown         (key_state.shiftl, VK_LSHIFT), _shiftDown         (key_state.shiftr, VK_RSHIFT)
#define shiftUp()            _shiftUp           (key_state.shift , VK_SHIFT ), _shiftUp           (key_state.shiftl, VK_LSHIFT), _shiftUp           (key_state.shiftr, VK_RSHIFT)

void onClipboardUpdate() {
	if (!onupdate_enabled) {
		onupdate_enabled = true;
		return;
	}
	
	auto& current = current_is_a ? clipboard_a : clipboard_b;
	auto r = Clipboard :: get();
	if (r.ok) { current = r.data; }
	
	if (should_shift_down) {
		should_shift_down = false;
		shiftDownWithAsync();
	}
}

KeyHookResult onTargetKeys(DWORD vk_code, bool shift, int nCode, WPARAM wParam, LPARAM lParam) {
	if (vk_code == 'C' || vk_code == 'X') {
		if (shift) {
			current_is_a = false;
			shiftUp();
			should_shift_down = true;
		} else {
			current_is_a = true;
		}
		return NEXT_HOOK;
	} else if (vk_code == 'V') {
		bool need_swap = (shift == current_is_a);
		
		if (need_swap) {
			auto& target = shift ? clipboard_b : clipboard_a;
			current_is_a = !shift;
			
			onupdate_enabled = false;
			auto r = Clipboard :: set(target);
			//	onupdate_enabled = true;
			if (!r) {
				onupdate_enabled = true;
				return BLOCK;
			}
		}
		
		if (shift) {
			shiftUp();
			LRESULT result = CallNextHookEx(g_keyboard_hook, nCode, wParam, lParam);
			shiftDown();
			return result;
		} else {
			return NEXT_HOOK;
		}
	}
	
	return NEXT_HOOK;
}
KeyHookResult onAltTarget(DWORD vk_code) {
	if (vk_code == 'X') { current_is_a = true         ; } else 
	if (vk_code == 'C') { current_is_a = !current_is_a; } else
	if (vk_code == 'V') { current_is_a = false        ; } else 
	{ return NEXT_HOOK; }
	return BLOCK;
}
