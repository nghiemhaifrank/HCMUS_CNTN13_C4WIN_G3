// QuickOpenFolder.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "QuickOpenFolder.h"
#include "MData.h"
#include <ShObjIdl.h>
#include <shellapi.h>
#include <string>
#include <Windowsx.h>

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	DlgShow(HWND, UINT, WPARAM, LPARAM);

void DrawClient(HDC hdc);
void HighLineRect(HDC hdc, POINT p);
TCHAR * BrowseFolder(std::string saved_path);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
					   _In_opt_ HINSTANCE hPrevInstance,
					   _In_ LPTSTR    lpCmdLine,
					   _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_QUICKOPENFOLDER, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_QUICKOPENFOLDER));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}


	MData::destroy();
	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_QUICKOPENFOLDER));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_QUICKOPENFOLDER);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;

	hInst = hInstance; // Store instance handle in our global variable

	hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
		return FALSE;
	}
	RECT rect;
	HWND hDesktop = GetDesktopWindow();
	GetClientRect(hDesktop, &rect);
	int LLL = MData::N*(MData::L + 2 * MData::l);
	SetWindowPos(hWnd, HWND_TOP,  rect.right/2-LLL/2, rect.bottom/2-LLL/2, LLL+15, LLL+55, SWP_SHOWWINDOW);
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}



//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	static POINT A;
	static POINT B;
	MData * data = MData::getInstance();

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case ID_PATH_NEW:
			{
				WCHAR * path = BrowseFolder("c:\\");
				if (data->addCurPath(path))
					MessageBox(hWnd, L"Create new short path successfully", L"Message", MB_OK);
				else
					MessageBox(hWnd, L"Create new short path fail", L"Message", MB_OK);
			}
			break;
		case ID_PATH_SHOW:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_DLGSHOW), hWnd, DlgShow);
			break;
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_LBUTTONDOWN:
		{
			POINT p = {LOWORD(lParam), HIWORD(lParam)};
			data->setObj(new MFreePen(p));
		}
		break;
	case WM_MOUSEMOVE:
		if (!(wParam & WM_LBUTTONDOWN))
			break;
		{
			POINT p = {LOWORD(lParam), HIWORD(lParam)};
			data->getObj()->doHandleMove(hWnd, p);
			p = data->trans2MapP(p);
			if (data->isMapPoint(p)){
				hdc = GetDC(hWnd);
				HighLineRect(hdc, p);
				ReleaseDC(hWnd, hdc);
				data->addMapPoint(p);
			}
		}
		break;
	case WM_LBUTTONUP:
		{
			WCHAR * dir = data->findSymbol();
			if (dir)
				ShellExecute(NULL, L"open", dir, NULL, NULL, SW_SHOWDEFAULT);
		}
		hdc = GetDC(hWnd);
		DrawClient(hdc);
		ReleaseDC(hWnd, hdc);
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		DrawClient(hdc);
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

INT_PTR CALLBACK DlgShow(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HWND hEDir;
	static HWND hLPath;
	static HWND hBtnDel;
	MData * data = MData::getInstance();
	int id;
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		hEDir = GetDlgItem(hDlg, IDC_EDIR);
		hLPath = GetDlgItem(hDlg, IDC_LISTPATH);
		hBtnDel = GetDlgItem(hDlg, IDC_BTNDEL);
		{
			int n = data->getNPaths();
			for (int i = 0; i < n; i++){
				WCHAR * buff = data->getPreviewPath(i);
				SendMessage(hLPath, LB_ADDSTRING, 0, (LPARAM) buff);
				delete buff;
			}
		}
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		id = LOWORD(wParam);
		switch (id)
		{
		case IDC_BTNDEL:
			{
				int pos = ListBox_GetCurSel(hLPath);
				if (pos != LB_ERR)
				{
					ListBox_DeleteString(hLPath, pos);
					data->delPath(pos);
				}
			}
			break;
		case IDC_BTNGETDIR:
			{
				int pos = ListBox_GetCurSel(hLPath);
				if (pos != LB_ERR)
					Edit_SetText(hEDir, data->getDir(pos));
			}
			break;
		case IDOK:
		case IDCANCEL:
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		default:
			break;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

void DrawClient(HDC hdc){
	int LL = MData::L + 2*MData::l;
	int l = MData::l;
	for(int i = 0; i < MData::N; i++)
		for (int j = 0; j < MData::N; j++)
		{
			Rectangle(hdc, LL*i, LL*j, LL*(i+1)-1, LL*(j+1)-1);
		}
		for(int i = 0; i < MData::N; i++)
			for (int j = 0; j < MData::N; j++)
			{
				Rectangle(hdc, LL*i+l, LL*j+l, LL*(i+1)-l, LL*(j+1)-l);
			}
}

void HighLineRect(HDC hdc, POINT p){
	SelectObject(hdc, GetStockObject(DC_BRUSH));
	SetDCBrushColor(hdc, RGB(255, 0, 0));
	int LL = MData::L + 2*MData::l;
	int l = MData::l;
	Rectangle(hdc, LL*p.x+l, LL*p.y+l, LL*(p.x+1)-l, LL*(p.y+1)-l);
}

#include <windows.h>
#include <string>
#include <shlobj.h>
#include <iostream>
#include <sstream>

static int CALLBACK BrowseCallbackProc(HWND hwnd,UINT uMsg, LPARAM lParam, LPARAM lpData)
{

	if(uMsg == BFFM_INITIALIZED)
	{
		std::string tmp = (const char *) lpData;
		std::cout << "path: " << tmp << std::endl;
		SendMessage(hwnd, BFFM_SETSELECTION, TRUE, lpData);
	}

	return 0;
}

WCHAR * BrowseFolder(std::string saved_path)
{
	WCHAR * path = new WCHAR[MAX_PATH];

	const char * path_param = saved_path.c_str();

	BROWSEINFO bi = { 0 };
	bi.lpszTitle  = L"Browse for folder...";
	bi.ulFlags    = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
	bi.lpfn       = BrowseCallbackProc;
	bi.lParam     = (LPARAM) path_param;

	LPITEMIDLIST pidl = SHBrowseForFolder ( &bi );

	if ( pidl != 0 )
	{
		//get the name of the folder and put it in path
		SHGetPathFromIDList ( pidl, path );

		//free memory used
		IMalloc * imalloc = 0;
		if ( SUCCEEDED( SHGetMalloc ( &imalloc )) )
		{
			imalloc->Free ( pidl );
			imalloc->Release ( );
		}

		return path;
	}

	return L"";
}