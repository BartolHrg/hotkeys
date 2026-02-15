#include "./ClipboardManager.hpp"
#include "./ClipboardUtils.cpp"
#include "./CppPatterns.hpp"

static std :: vector<Clipboard :: AllData> clipboards;
static ssize_t next_index = 0;
static bool onupdate_enabled = true;
enum InsertMode : bool {
	END,
	PLACE,
};
InsertMode insert_mode = InsertMode :: END;

bool init() {
	#ifdef SAVE_INITIAL
		auto data = Clipboard :: get();
		if (!data.ok) { return false; }
		clipboards.push_back(data.data);
	#endif
	return true;
}

void onClipboardUpdate() {
	if (!onupdate_enabled) {
		onupdate_enabled = true;
		return;
	}
	
	auto r = Clipboard :: get();
	if (r.ok) {
		if (insert_mode == InsertMode :: END || next_index == clipboards.size()) {
			clipboards.push_back(r.data);
		} else {
			clipboards.insert(clipboards.begin() + next_index, r.data);
		}
	}
	//	LOG("After Clipboard Update : %lld / %zu", next_index, clipboards.size());
}

KeyHookResult onTargetKeys(DWORD vk_code, bool shift, int, WPARAM, LPARAM) {
	if (shift) {
		if (vk_code == 'C') {
			if (--next_index == -1) {
				next_index = clipboards.size() - 1;
			}
		} else if (vk_code == 'X') {
			clipboards.pop_back();
			auto n = clipboards.size();
			if (n == 0) {
				next_index = 0;
			} else if (next_index == n + 1) {
				--next_index;
			}
		} else if (vk_code == 'V') {
			if (++next_index == clipboards.size() + 1) {
				next_index = 1;
			}
		}
		//	LOG("After Ctrl Shift %c : %lld / %zu", (int) vk_code, next_index, clipboards.size());
		return BLOCK;
	} else {
		if (vk_code == 'V') {
			auto n = clipboards.size();
			if (n == 0) {
				return BLOCK;
			}
			if (next_index == n) {
				next_index = 0;
			}
			onupdate_enabled = false;
			auto r = Clipboard :: set(clipboards[next_index]);
			//	onupdate_enabled = true;
			if (!r) {
				onupdate_enabled = true;
				return BLOCK;
			}
			if (++next_index == n + 1) {
				next_index = 1;
			}
		}
		//	LOG("After Ctrl       %c : %lld / %zu", (int) vk_code, next_index, clipboards.size());
		return NEXT_HOOK;
	}
}
KeyHookResult onAltTarget(DWORD vk_code) {
	if (vk_code == 'X') { insert_mode = InsertMode :: END                    ; } else 
	if (vk_code == 'C') { insert_mode = static_cast<InsertMode>(!insert_mode); } else
	if (vk_code == 'V') { insert_mode = InsertMode :: PLACE                  ; } else 
	{ return NEXT_HOOK; }
	return BLOCK;
}
