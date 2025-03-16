// https://learn.microsoft.com/ja-jp/windows/win32/controls/tab-control-reference

// https://wisdom.sakura.ne.jp/system/winapi/common/common11.html


#include <windows.h>
#include <commctrl.h>
#include <stdio.h>

////////////////////
// VT Window

#define VTClassName L"VTWin32"

LRESULT CALLBACK vtwin_proc(HWND hWnd , UINT msg , WPARAM wp , LPARAM lp)
{
	switch(msg)
	{
	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC PaintDC = BeginPaint(hWnd, &ps);
		HBRUSH b = CreateSolidBrush(RGB(0, 0xff, 0));
		SelectObject(PaintDC, (HGDIOBJ)b);

		// 全体を描画
		RECT rc;
		GetClientRect(hWnd, &rc);
		Rectangle(PaintDC, rc.left, rc.top, rc.right, rc.bottom);

		ReleaseDC(hWnd, PaintDC);
		EndPaint(hWnd, &ps);
		break;
	}
	case WM_SIZE:
	case WM_MOVE:
		InvalidateRect(hWnd, NULL, FALSE);
		break;
	}
	return DefWindowProc(hWnd , msg , wp , lp);
}

HWND vtwin(HINSTANCE hInstance, HWND hParentWnd)
{
	WNDCLASSW wc = {};

	wc.style = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = vtwin_proc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon			= LoadIcon(NULL , IDI_APPLICATION);
	wc.hCursor = NULL;
	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = VTClassName;

	RegisterClassW(&wc);

	HWND hWnd = CreateWindowExW(
		0,
		VTClassName,
		L"VTWin",
		WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		hParentWnd,
		NULL,
		hInstance,
		NULL);

	ShowWindow(hWnd, SW_SHOW);

	return hWnd;
}

//////////////////// ↑VTWindows

/**
 *	タブとVTWindowの位置を調整する
 *
 *	@param	hWnd	親ウィンドウ
 *	@param	hTab	タブコントロール
 *	@param	hVTWnd	VTWindow
 */
static void vtwin_pos(HWND hWnd, HWND hTab, HWND hVTWnd)
{
	RECT rc;
	GetClientRect(hWnd, &rc);

	// タブコントロールの高さを取得
	RECT itemRect;
	TabCtrl_GetItemRect(hTab, 0, &itemRect);
	int tabHeight = itemRect.bottom - itemRect.top;
	int client_width = rc.right - rc.left;
	MoveWindow(hTab, 0, 0, client_width, tabHeight, TRUE);

	// タブ分を引いた領域
	TabCtrl_AdjustRect(hTab, FALSE, &rc);

	// VT Window 位置調整
	if (hVTWnd != NULL) {
		int width = rc.right - rc.left;
		int height = rc.bottom - rc.top;
		MoveWindow(hVTWnd, rc.left, rc.top, width, height, TRUE);
	}
}

typedef struct {
	HWND hVTWnd;
	HWND hTab;
} win_data_t;

LRESULT CALLBACK WndProc(HWND hWnd , UINT msg , WPARAM wp , LPARAM lp)
{
	switch (msg) {
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_CREATE: {
		CREATESTRUCT *p = (CREATESTRUCT *)lp;
		win_data_t *data = (win_data_t *)p->lpCreateParams;
		SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)data);

		// タブコントロール
		HWND hTab;
		hTab = CreateWindowExW(0, WC_TABCONTROLW, NULL,
			WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
			0, 0, 10, 10, hWnd, (HMENU)0x10,
			p->hInstance, NULL);
		data->hTab = hTab;

		TCITEM tc_item;
		tc_item.mask = TCIF_TEXT;
		tc_item.pszText = L"tab1";
		TabCtrl_InsertItem(hTab , 0 , &tc_item);

		tc_item.pszText = L"tab2";
		TabCtrl_InsertItem(hTab , 1 , &tc_item);

		return 0;
	}
	case WM_SIZE: {
		win_data_t *data = (win_data_t *)GetWindowLongPtr(hWnd, GWLP_USERDATA);

		if (data->hVTWnd != NULL) {
			vtwin_pos(hWnd, data->hTab, data->hVTWnd);
		}

		return 0;
	}
	case WM_NOTIFY: {
		win_data_t *data = (win_data_t *)GetWindowLongPtr(hWnd, GWLP_USERDATA);
		switch (((LPNMHDR)lp)->code) {
		case TCN_SELCHANGE: {
			int tab_no = TabCtrl_GetCurSel(data->hTab);
			char s[64];
			sprintf_s(s, "tab %d\n", tab_no);
			OutputDebugStringA(s);
			break;
		}
		}
		break;
	}
	}
	return DefWindowProc(hWnd , msg , wp , lp);
}

int WINAPI WinMain(HINSTANCE hInstance , HINSTANCE hPrevInstance ,
				   PSTR lpCmdLine , int nCmdShow )
{
	InitCommonControls();

	WNDCLASSW wc;
	wc.style			= CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc		= WndProc;
	wc.cbClsExtra       = 0;
	wc.cbWndExtra       = 0;
	wc.hInstance		= hInstance;
	wc.hIcon			= LoadIcon(NULL , IDI_APPLICATION);
	wc.hCursor			= LoadCursor(NULL , IDC_ARROW);
	wc.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName		= NULL;
	wc.lpszClassName	= L"tab_test";

	if (!RegisterClassW(&wc)) {
		return -1;
	}

	win_data_t data = {};

	HWND hWnd = CreateWindowW(
		L"tab_test", L"tab test windows title",
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, hInstance, &data);

	if (hWnd == NULL) {
		return -1;
	}

	data.hVTWnd = vtwin(hInstance, hWnd);
	vtwin_pos(hWnd, data.hTab, data.hVTWnd);

	ShowWindow(hWnd, SW_SHOW);

	{
		MSG msg;
		while(GetMessageW(&msg , NULL , 0 , 0)) {
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
		return (int)msg.wParam;
	}
}
