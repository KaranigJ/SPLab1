// lab1.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "lab1.h"

#define MAX_LOADSTRING 100
HANDLE thread1, thread2, thread3;

// Глобальные переменные:
HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_LAB1, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LAB1));

    MSG msg;

    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}

struct ThreadArgs
{
    int x, y;
} line1, line2, line3;

DWORD WINAPI MoveLine(ThreadArgs* args)
{
    while (true)
    {
        if (args->y == 390) args->y = 0;
        args->y += 10;
        Sleep(90);
    }
    return 0;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LAB1));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_LAB1);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // Store instance handle in our global variable

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
    HWND hStart = CreateWindow(L"button", L"start",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        10, 10, 70, 50, hWnd, (HMENU)ID_BUTTON_START, hInst, NULL);
    HWND hStop = CreateWindow(L"button", L"stop",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        10, 70, 70, 50, hWnd, (HMENU)ID_BUTTON_STOP, hInst, NULL);

    line1.x = 200;
    line1.y = 0;
    line2.x = 400;
    line2.y = 0;
    line3.x = 600;
    line3.y = 0;

    thread1 = CreateThread(NULL, 0u, (LPTHREAD_START_ROUTINE)MoveLine, &line1, 0, NULL);
    thread2 = CreateThread(NULL, 0u, (LPTHREAD_START_ROUTINE)MoveLine, &line2, 0, NULL);
    thread3 = CreateThread(NULL, 0u, (LPTHREAD_START_ROUTINE)MoveLine, &line3, 0, NULL);

    SetTimer(hWnd, NULL, 20, NULL);

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_TIMER:
        InvalidateRect(hWnd, NULL, true);
        break;
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case ID_BUTTON_START:
            ResumeThread(thread1);
            ResumeThread(thread2);
            ResumeThread(thread3);
            break;
        case ID_BUTTON_STOP:
            SuspendThread(thread1);
            SuspendThread(thread2);
            SuspendThread(thread3);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        if (line1.y != 0)
        {
            HPEN pen = CreatePen(BS_SOLID, 5, RGB(255, 0, 0));
            SelectObject(hdc, pen);
            MoveToEx(hdc, line1.x, line1.y, NULL);
            LineTo(hdc, line1.x, line1.y + 100);
            MoveToEx(hdc, line2.x, line2.y, NULL);
            LineTo(hdc, line2.x, line2.y + 100);
            MoveToEx(hdc, line3.x, line3.y, NULL);
            LineTo(hdc, line3.x, line3.y + 100);
        }
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

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

