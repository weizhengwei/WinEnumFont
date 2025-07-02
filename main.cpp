#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#ifndef UNICODE
#define UNICODE
#endif

#ifndef _UNICODE
#define _UNICODE
#endif

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <tchar.h>
#include <CommCtrl.h>
#include <string>
#include <vector>
#include <map>
#include <bitset>

namespace std {
	using tstring = std::basic_string<TCHAR>;
}

#ifdef _MSC_VER
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "comctl32.lib")
#endif

#define ID_LISTVIEW 1001
HWND hListView;

const TCHAR* CLASSNAME = _T("CLSNAME_ENUMFONT");
const TCHAR* WIN_TITLE = _T("WinEnumFont");
const TCHAR* VER = _T("V1.0.0");

std::vector<std::tstring> vecCols{
	_T("FaceName"),
	_T("Width"),
	_T("Height"),//字体的高度，以逻辑单位表示。如果是负值，表示字体的高度向下延伸
	_T("Weight"),
	_T("Escapement"),//平均宽度，以逻辑单位表示。如果为 0，表示使用默认宽度
	_T("Orientation"),//字符的倾斜角度（以度为单位），表示文本的水平倾斜方向
	_T("Underline"),
	_T("StrikeOut"),
	_T("CharSet"),
	_T("OutPrecision"),//输出精度
	_T("ClipPrecision"),//剪裁精度
	_T("Quality"),//质量
	_T("PitchAndFamily")//字体的间距和家族信息
};

std::map<int, int> vecColWidth = {
	{0, 250},
	{1, 50},
	{2, 50},
	{3, 150},
	{4, 100},
	{5, 80},
	{6, 80},
	{7, 80},
	{8, 200},
	{9, 100},
	{10, 100},
	{11, 80},
	{12, 300}
};

#define IDM_FILE_REFRESH 1001
#define IDM_FILE_EXIT 1002
#define IDM_HELP_ABOUT 2001

LRESULT CALLBACK WinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void QuitApp();
void HandleError(HWND hwnd, LPCTSTR lpszFunc);
void CreateMainMenu(HWND hwnd);
void CreateListView(HWND hwnd);
void FillDataToListView();
std::tstring GetCharSetStr(BYTE charset);
std::tstring GetWeightStr(LONG weight);
std::tstring GetPitchAndFamilyStr(BYTE pitchAndFamily);

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE hPreInstance, LPTSTR lpCmdLine, int nShowCmd)
{
	INITCOMMONCONTROLSEX iccex;
	iccex.dwICC = ICC_LISTVIEW_CLASSES;
	InitCommonControlsEx(&iccex);
	WNDCLASS wc;
	ZeroMemory(&wc, sizeof(wc));
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WinProc;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszClassName = CLASSNAME;
	if (RegisterClass(&wc) == 0)
	{
		HandleError(NULL, _T("RegisterClass"));
		return 1;
	}
	HWND hwnd = CreateWindowEx(WS_EX_APPWINDOW,
		CLASSNAME,
		WIN_TITLE,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		NULL,
		NULL,
		hInstance,
		NULL);
	CreateMainMenu(hwnd);
	CreateListView(hwnd);
	ShowWindow(hwnd, SW_SHOWDEFAULT);
	UpdateWindow(hwnd);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		DispatchMessage(&msg);
		TranslateMessage(&msg);
	}
	return (int)msg.wParam;
}

LRESULT CALLBACK WinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_COMMAND:
	{
		if (LOWORD(wParam) == IDM_FILE_REFRESH)
		{
			ListView_DeleteAllItems(hListView);
			UpdateWindow(hListView);
			FillDataToListView();
		}
		else if (LOWORD(wParam) == IDM_FILE_EXIT)
		{
			QuitApp();
		}
		else if (LOWORD(wParam) == IDM_HELP_ABOUT)
		{
			TCHAR szTitle[256] = { 0 };
			_stprintf_s(szTitle, ARRAYSIZE(szTitle), _T("%s %s\n\nCopyright(C) 2025 hostzhengwei@gmail.com"), WIN_TITLE, VER);
			MessageBox(hwnd, szTitle, WIN_TITLE, MB_OK);
		}
	}
	case WM_SIZE:
	{
		RECT rcClient;
		GetClientRect(hwnd, &rcClient);
		InflateRect(&rcClient, -15, -15);
		SetWindowPos(hListView,
			NULL,
			rcClient.left,
			rcClient.top,
			rcClient.right - rcClient.left,
			rcClient.bottom - rcClient.top,
			//SWP_SHOWWINDOW
			SWP_NOACTIVATE | SWP_NOZORDER
		);
		break;
	}
	case WM_DESTROY:
	{
		QuitApp();
		break;
	}
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

void QuitApp()
{
	PostQuitMessage(0);
}

void HandleError(HWND hwnd, LPCTSTR lpszFunc)
{
	LPVOID lpMsgBuf;
	LPVOID lpDisplayBuf;
	DWORD dwErr = GetLastError();

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dwErr,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0, NULL);

	// Display the error message and exit the process.

	lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, (lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunc) + 40) * sizeof(TCHAR));
	if (!lpDisplayBuf) return;
	//StringCchPrintf
	_stprintf_s((LPTSTR)lpDisplayBuf,
		LocalSize(lpDisplayBuf) / sizeof(TCHAR),
		TEXT("%s failed with error %d: %s"),
		lpszFunc, dwErr, (LPCTSTR)lpMsgBuf);
	MessageBox(hwnd, (LPCTSTR)lpDisplayBuf, WIN_TITLE, MB_OK);

	LocalFree(lpMsgBuf);
	LocalFree(lpDisplayBuf);
}

void CreateMainMenu(HWND hwnd)
{
	HMENU hMainMenu = CreateMenu();
	HMENU hFileSubMenu = CreatePopupMenu();
	AppendMenu(hFileSubMenu, MF_STRING, IDM_FILE_REFRESH, _T("Refresh"));
	AppendMenu(hFileSubMenu, MF_SEPARATOR, 0, _T(""));
	AppendMenu(hFileSubMenu, MF_STRING, IDM_FILE_EXIT, _T("Exit"));

	HMENU hHelpSubMenu = CreatePopupMenu();
	AppendMenu(hHelpSubMenu, MF_STRING, IDM_HELP_ABOUT, _T("About"));

	AppendMenu(hMainMenu, MF_POPUP, (UINT_PTR)hFileSubMenu, _T("File"));
	AppendMenu(hMainMenu, MF_POPUP, (UINT_PTR)hHelpSubMenu, _T("Help"));
	SetMenu(hwnd, hMainMenu);
}

void CreateListView(HWND hwnd)
{
	RECT rc;
	GetClientRect(hwnd, &rc);
	InflateRect(&rc, -10, -10);
	hListView = CreateWindowEx(
		WS_EX_CLIENTEDGE,
		WC_LISTVIEW,
		NULL,
		WS_VISIBLE | WS_CHILD | WS_CLIPCHILDREN | LVS_REPORT | LVS_NOSORTHEADER,
		rc.left,
		rc.top,
		rc.right - rc.left,
		rc.bottom - rc.top,
		hwnd,
		(HMENU)ID_LISTVIEW,
		GetModuleHandle(NULL),
		NULL);
	ListView_SetTextColor(hListView, RGB(10, 10, 160));
	ListView_SetExtendedListViewStyle(hListView, LVS_EX_FULLROWSELECT);

	TCHAR szText[MAX_PATH] = {};
	LVCOLUMN lvCol;
	lvCol.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvCol.fmt = LVCFMT_LEFT;
	lvCol.cxDefault = 150;
	lvCol.pszText = szText;

	for (int iCol = 0; iCol < vecCols.size(); iCol++)
	{
		lvCol.cx = vecColWidth.at(iCol);
		_stprintf_s(szText, MAX_PATH, _T("%s"), vecCols[iCol].c_str());
		ListView_InsertColumn(hListView, iCol, &lvCol);
	}

	FillDataToListView();
}

void FillDataToListView()
{
	HDC hdc = GetDC(GetDesktopWindow());
	LOGFONT lf = {};
	lf.lfCharSet = DEFAULT_CHARSET;
	EnumFontFamiliesEx(hdc, &lf, [](const LOGFONT* lpelfe, const TEXTMETRIC* lpntme, DWORD FontType, LPARAM lParam)->int {
		LVITEM lvItem = {};
		TCHAR szText[128] = {};

		lvItem.iItem = ListView_GetItemCount(hListView);
		lvItem.iSubItem = 0;
		_stprintf_s(szText, ARRAYSIZE(szText), _T("%s"), lpelfe->lfFaceName);
		lvItem.mask = LVIF_TEXT;
		lvItem.pszText = szText;
		ListView_InsertItem(hListView, &lvItem);

		lvItem.iSubItem = 1;
		_stprintf_s(szText, ARRAYSIZE(szText), _T("%d"), lpelfe->lfWidth);
		lvItem.mask = LVIF_TEXT;
		lvItem.pszText = szText;
		ListView_SetItem(hListView, &lvItem);

		lvItem.iSubItem = 2;
		_stprintf_s(szText, ARRAYSIZE(szText), _T("%d"), lpelfe->lfHeight);
		lvItem.mask = LVIF_TEXT;
		lvItem.pszText = szText;
		ListView_SetItem(hListView, &lvItem);

		lvItem.iSubItem = 3;
		_stprintf_s(szText, ARRAYSIZE(szText), _T("%d [%s]"), lpelfe->lfWeight, GetWeightStr(lpelfe->lfWeight).c_str());
		lvItem.mask = LVIF_TEXT;
		lvItem.pszText = szText;
		ListView_SetItem(hListView, &lvItem);

		lvItem.iSubItem = 4;
		_stprintf_s(szText, ARRAYSIZE(szText), _T("%d"), lpelfe->lfEscapement);
		lvItem.mask = LVIF_TEXT;
		lvItem.pszText = szText;
		ListView_SetItem(hListView, &lvItem);

		lvItem.iSubItem = 5;
		_stprintf_s(szText, ARRAYSIZE(szText), _T("%d"), lpelfe->lfOrientation);
		lvItem.mask = LVIF_TEXT;
		lvItem.pszText = szText;
		ListView_SetItem(hListView, &lvItem);

		lvItem.iSubItem = 6;
		_stprintf_s(szText, ARRAYSIZE(szText), _T("%d"), lpelfe->lfUnderline);
		lvItem.mask = LVIF_TEXT;
		lvItem.pszText = szText;
		ListView_SetItem(hListView, &lvItem);

		lvItem.iSubItem = 7;
		_stprintf_s(szText, ARRAYSIZE(szText), _T("%d"), lpelfe->lfStrikeOut);
		lvItem.mask = LVIF_TEXT;
		lvItem.pszText = szText;
		ListView_SetItem(hListView, &lvItem);

		lvItem.iSubItem = 8;
		_stprintf_s(szText, ARRAYSIZE(szText), _T("%d [%s]"), lpelfe->lfCharSet, GetCharSetStr(lpelfe->lfCharSet).c_str());
		lvItem.mask = LVIF_TEXT;
		lvItem.pszText = szText;
		ListView_SetItem(hListView, &lvItem);

		lvItem.iSubItem = 9;
		_stprintf_s(szText, ARRAYSIZE(szText), _T("%d"), lpelfe->lfOutPrecision);
		lvItem.mask = LVIF_TEXT;
		lvItem.pszText = szText;
		ListView_SetItem(hListView, &lvItem);

		lvItem.iSubItem = 10;
		_stprintf_s(szText, ARRAYSIZE(szText), _T("%d"), lpelfe->lfClipPrecision);
		lvItem.mask = LVIF_TEXT;
		lvItem.pszText = szText;
		ListView_SetItem(hListView, &lvItem);

		lvItem.iSubItem = 11;
		_stprintf_s(szText, ARRAYSIZE(szText), _T("%d"), lpelfe->lfQuality);
		lvItem.mask = LVIF_TEXT;
		lvItem.pszText = szText;
		ListView_SetItem(hListView, &lvItem);

		lvItem.iSubItem = 12;
		_stprintf_s(szText, ARRAYSIZE(szText), _T("%d [%s]"), lpelfe->lfPitchAndFamily, GetPitchAndFamilyStr(lpelfe->lfPitchAndFamily).c_str());
		lvItem.mask = LVIF_TEXT;
		lvItem.pszText = szText;
		ListView_SetItem(hListView, &lvItem);

		return 1;
	}, NULL, 0);
}

std::tstring GetCharSetStr(BYTE charset)
{
	switch (charset) {
	case ANSI_CHARSET:
		return _T("ANSI_CHARSET");
	case DEFAULT_CHARSET:
		return _T("DEFAULT_CHARSET");
	case SYMBOL_CHARSET:
		return _T("SYMBOL_CHARSET");
	case SHIFTJIS_CHARSET:
		return _T("SHIFTJIS_CHARSET");
	case HANGEUL_CHARSET:
		return _T("HANGUL_CHARSET");
	case GB2312_CHARSET:
		return _T("GB2312_CHARSET");
	case CHINESEBIG5_CHARSET:
		return _T("CHINESEBIG5_CHARSET");
	case OEM_CHARSET:
		return _T("OEM_CHARSET");
#if (WINVER >= 0x0400)
	case JOHAB_CHARSET:
		return _T("JOHAB_CHARSET");
	case HEBREW_CHARSET:
		return _T("HEBREW_CHARSET");
	case ARABIC_CHARSET:
		return _T("ARABIC_CHARSET");
	case GREEK_CHARSET:
		return _T("GREEK_CHARSET");
	case TURKISH_CHARSET:
		return _T("TURKISH_CHARSET");
	case VIETNAMESE_CHARSET:
		return _T("VIETNAMESE_CHARSET");
	case THAI_CHARSET:
		return _T("THAI_CHARSET");
	case EASTEUROPE_CHARSET:
		return _T("EASTEUROPE_CHARSET");
	case RUSSIAN_CHARSET:
		return _T("RUSSIAN_CHARSET");
	case MAC_CHARSET:
		return _T("MAC_CHARSET");
	case BALTIC_CHARSET:
		return _T("BALTIC_CHARSET");
#endif
	default:
		return _T("UNKNOWN_CHARSET");
	}
}

std::tstring GetWeightStr(LONG weight)
{
	switch (weight) {
	case FW_DONTCARE:
		return _T("FW_DONTCARE");
	case FW_THIN:
		return _T("FW_THIN");
	case FW_EXTRALIGHT:
		return _T("FW_EXTRALIGHT");
	case FW_LIGHT:
		return _T("FW_LIGHT");
	case FW_NORMAL:
		return _T("FW_NORMAL");
	case FW_MEDIUM:
		return _T("FW_MEDIUM");
	case FW_SEMIBOLD:
		return _T("FW_SEMIBOLD");
	case FW_BOLD:
		return _T("FW_BOLD");
	case FW_EXTRABOLD:
		return _T("FW_EXTRABOLD");
	case FW_HEAVY:
		return _T("FW_HEAVY");
	default:
		return _T("UNKNOWN_WEIGHT");
	}
}
//https://learn.microsoft.com/en-us/windows/win32/api/wingdi/ns-wingdi-logfonta
std::tstring GetPitchAndFamilyStr(BYTE pitchAndFamily)
{
	BYTE pitch = (pitchAndFamily & 0b11);
	BYTE family = (pitchAndFamily & 0xF0 );

	const TCHAR* pitchStr;
	switch (pitch) {
	case DEFAULT_PITCH:
		pitchStr = _T("DEFAULT_PITCH");
		break;
	case FIXED_PITCH:
		pitchStr = _T("FIXED_PITCH");
		break;
	case VARIABLE_PITCH:
		pitchStr = _T("VARIABLE_PITCH");
		break;
	default:
		pitchStr = _T("UNKNOWN_PITCH");
		break;
	}
	const TCHAR* familyStr;
	switch (family) {
	case FF_DONTCARE:
		familyStr = _T("FF_DONTCARE");
		break;
	case FF_ROMAN:
		familyStr = _T("FF_ROMAN");
		break;
	case FF_SWISS:
		familyStr = _T("FF_SWISS");
		break;
	case FF_MODERN:
		familyStr = _T("FF_MODERN");
		break;
	case FF_SCRIPT:
		familyStr = _T("FF_SCRIPT");
		break;
	case FF_DECORATIVE:
		familyStr = _T("FF_DECORATIVE");
		break;
	default:
		familyStr = _T("UNKNOWN_FAMILY");
		break;
	}

	TCHAR result[256];
	_stprintf_s(result, _T("Pitch: %s, Family: %s"), pitchStr, familyStr);
	return result;
}