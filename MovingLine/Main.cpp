#include <Windows.h>
#include "MovingLine.h"

#define TRACE_L 15
#define LINE_L 90
#define DIST 15
#define SPEED 10

char szClassName[] = "Window1";
HWND hWnd;
char fileName[300] = { 0 };

LRESULT CALLBACK WndProc(HWND, UINT, UINT, LONG);
ATOM registerMyClass(HINSTANCE hInstance);
int createMyWindow(HINSTANCE hInstance, int nCmdShow);

MovingLine* pMline = NULL;

BOOL DrawLine(HDC hdc, int x1, int y1, int x2, int y2)
{
	POINT pt;
	MoveToEx(hdc, x1, y1, &pt);
	return LineTo(hdc, x2, y2);
}

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpszCmdParam, _In_ int nCmdShow)
{
	MSG msg;
	pMline = new MovingLine(SPEED, TRACE_L, LINE_L, DIST);
	
	createMyWindow(hInstance, nCmdShow);
	
	while (GetMessage(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CREATE: {
		SetTimer(hWnd, 1, 10, NULL);
		break;
	}
	case WM_TIMER: {
		pMline->MoveLines();
		InvalidateRect(hWnd, NULL, TRUE);
		break;
	}
	case WM_DESTROY: {
		delete pMline;
		PostQuitMessage(0);
		break;
	}
	case WM_KEYDOWN: {
		switch (wParam)	{
		case VK_UP:
			pMline->ChangeDir(Dir::UP);
			break;
		case VK_DOWN:
			pMline->ChangeDir(Dir::DOWN);
			break;
		case VK_RIGHT:
			pMline->ChangeDir(Dir::RIGHT);
			break;
		case VK_LEFT:
			pMline->ChangeDir(Dir::LEFT);
			break;
		}
		break;
	}
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		pMline->Draw(hdc);
		EndPaint(hWnd, &ps);
	}
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

int createMyWindow(HINSTANCE hInstance, int nCmdShow)
{
	registerMyClass(hInstance);

	hWnd = CreateWindow(szClassName, "MovingLine", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 1200, 900, NULL, NULL, hInstance, NULL);

	if (!hWnd) { return 0; }
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);


}

ATOM registerMyClass(HINSTANCE hInstance)
{
	WNDCLASS wc;
	wc.cbClsExtra = wc.cbWndExtra = 0;
	wc.style = 0;
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = szClassName;

	return RegisterClass(&wc);
}