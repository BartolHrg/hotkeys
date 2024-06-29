#define UNICODE
#define _UNICODE
#define STRICT
#include <windows.h>
#include <stdio.h>


#define MY_X 1850
#define MY_Y 60
#define MY_W 65
#define MY_H 140

HWND osd_hwnd;
FILE* log_file;
// #define println(fmt, ...) fprintf(log_file, fmt "\n", ## __VA_ARGS__); fflush(log_file);
#define println(fmt, ...) 

void getOsdHwnd() {
	while (1) {
		HWND hwnd0 = FindWindowEx(NULL, NULL , L"NativeHWNDHost", NULL);
		if (!hwnd0) continue;
		// debugHwnd(hwnd0);
		// who gives a fuck about children
		// HWND hwnd1 = FindWindowEx(NULL, hwnd0, "DirectUIHWND"	, NULL);
		// if (!hwnd1) exit(0); // continue;
		osd_hwnd = hwnd0;
		return;
	}
}
void correction() {
	SetWindowPos(osd_hwnd, NULL, MY_X, MY_Y, MY_W, MY_H, SWP_NOSIZE);
}
void CALLBACK onChangePosition(HWINEVENTHOOK hook, DWORD event, HWND hwnd, LONG idObject, LONG idChild, DWORD idEventThread, DWORD time) {
	if (hwnd == osd_hwnd && idObject == OBJID_WINDOW && idChild == CHILDID_SELF && event == EVENT_OBJECT_LOCATIONCHANGE) {
		RECT rc;
		if (!GetWindowRect(osd_hwnd, &rc)) {
			println("error get rect");
			return;
		}
		if (rc.left != MY_X || rc.top != MY_Y) {
			println("window rect is (%4d,%4d)-(%4d,%4d)", rc.left, rc.top, rc.right, rc.bottom);
			correction();
		}
		
	}
}
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
	// log_file = stdout;
	// log_file = fopen("./log.txt", "w");
	// if (!log_file) {
	// 	printf("Error log file\n");
	// 	return 1;
	// }
	FreeConsole();
	
	getOsdHwnd();
	DWORD processId;
	DWORD threadId = GetWindowThreadProcessId(osd_hwnd, &processId);
	HWINEVENTHOOK hook = SetWinEventHook(EVENT_OBJECT_LOCATIONCHANGE, EVENT_OBJECT_LOCATIONCHANGE, NULL, onChangePosition, processId, threadId, WINEVENT_OUTOFCONTEXT);
	// MessageBox(NULL, L"Press OK when bored", L"Title", MB_OK);

	correction();
	MSG msg;
	for (int x; x = GetMessage(&msg, NULL, 0, 0); ) {
		if (x == -1) { println("error get msg"); break; }
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	println("Done");
	UnhookWinEvent(hook);
	return 0;
}

void debugHwnd(HWND hwnd) {
	RECT rect;
	GetWindowRect(hwnd, &rect);
	int 
		x = rect.left,
		y = rect.top,
		w = rect.right - rect.left,
		h = rect.bottom - rect.top
	;
	println("(%d, %d) [%d x %d]", x, y, w, h);
}

	// ; hw_handle := DllCall( "FindWindowEx", "uint", 0        , "uint", 0, "str", "XamlExplorerHostIslandWindow", "uint", 0 )
	// ; hw_handle := DllCall( "FindWindowEx", "uint", hw_handle, "uint", 0, "str", "Windows.UI.Composition.DesktopWindowContentBridge", "str", "DesktopWindowXamlSource" )
