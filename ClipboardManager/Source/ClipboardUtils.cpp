#pragma once

#include <windows.h>
#include <string>
#include <vector>
#include "./CppPatterns.hpp"

namespace Clipboard {
	struct Data {
		UINT                format     ;
		std :: wstring      format_name;
		std :: vector<BYTE> content    ;
	};
	using AllData = std :: vector<Data>;
	
	constexpr UINT CF_CUSTOM = 0xC000;
	
	namespace {
		std :: wstring _getFormatName(UINT format) {
			if (format < CF_CUSTOM) { return {}; }
			size_t capacity = 128;
			std :: wstring result; result.resize(capacity);
			GetClipboardFormatNameW(format, result.data(), capacity - 1);
			return result;
		}
	}
	
	bool set(char* text) {
		if (!OpenClipboard(nullptr)) {
			return false;
		}
		defer { CloseClipboard(); };
		
		EmptyClipboard();
		
		size_t len = strlen(text) + 1;
		HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, len);
		if (!hMem) {
			return false;
		}
		
		char* pMem = (char*)GlobalLock(hMem);
		if (!pMem) {
			GlobalFree(hMem);
			return false;
		}
		
		memcpy(pMem, text, len);
		GlobalUnlock(hMem);
		
		if (!SetClipboardData(CF_TEXT, hMem)) {
			GlobalFree(hMem);
			return false;
		}
		
		return true;
	}
	
	bool set(AllData data) {
		if (!OpenClipboard(nullptr)) {
			return false;
		}
		defer { CloseClipboard(); };
		
		EmptyClipboard();
		
		for (auto& item : data) {
			if (item.content.empty()) {
				//	what if some program uses empty format just to mark something?
				continue;
			}
			HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, item.content.size());
			if (!hMem) {
				continue;
			}
			void* pMem = GlobalLock(hMem);
			if (!pMem) {
				GlobalFree(hMem);
				continue;
			}
			
			memcpy(pMem, item.content.data(), item.content.size());
			GlobalUnlock(hMem);
			
			if (!SetClipboardData(item.format, hMem)) {
				GlobalFree(hMem);
			}
		}
		
		return true;
	}
	
	Result<AllData> get() {
		AllData result;
		
		if (!OpenClipboard(nullptr)) {
			return Result<AllData> :: MY_NO_DATA;
		}
		defer { CloseClipboard(); };
		
		for (UINT format = 0; format = EnumClipboardFormats(format); ) {
			Data item;
			item.format = format;
			item.format_name = _getFormatName(format);
			
			HANDLE hData = GetClipboardData(format);
			if (!hData) {
				return Result<AllData> :: MY_NO_DATA;
			}
			SIZE_T size = GlobalSize(hData);
			if (size == 0) {
				continue;
			}
			void* pData = GlobalLock(hData);
			if (!pData) {
				return Result<AllData> :: MY_NO_DATA;
			}
			item.content.resize(size);
			memcpy(item.content.data(), pData, size);
			
			GlobalUnlock(hData);
			
			result.push_back(item);
		}
		
		return result;
	}
}