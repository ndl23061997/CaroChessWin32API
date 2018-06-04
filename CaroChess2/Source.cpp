#include <Windows.h>
#include <stdexcept>
using namespace std;
// Bien toan cuc
static HPEN penRED, penBLUE, penDEFAULT;
static float ScreenWidth, ScreenHeight, CellWidth, CellHeight;
static int M = 25; // so cot
static int N = 25; // so hang
static int map[25][25]; // Luu thong tin cua ban co, map[m][n] : m cot va n hang
static int status = 1; // trang thai X,O
static int Winner;
static int count, person;
// Dinh nghia ham
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
int DrawLine(HDC hdc, int xStart, int yStart, int xEnd, int yEnd);
int DrawX(HDC, int x, int y);
int Win(int x, int y, int person);

// Ham WinMain
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	// Khai bao bien.
	WNDCLASS wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[] = TEXT("Caro Chess");
	// Dang ki WindowsClass.
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = szAppName;
	if (!RegisterClass(&wndclass))
	{
		MessageBox(NULL, TEXT("This program requires Windows NT!"),
			szAppName, MB_ICONERROR);
		return 0;
	}
	// Tao cua so.
	hwnd = CreateWindow(szAppName,
		TEXT("Caro Chess"), // window caption
		WS_OVERLAPPEDWINDOW, // Kieu cua so
		CW_USEDEFAULT, // initial x position
		CW_USEDEFAULT, // initial y position
		800, // Chieu rong cua so
		800, // Chieu cao cua so
		NULL, // parent window handle
		NULL, // window menu handle
		hInstance, // program instance handle
		NULL); // creation parameters
	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);
	// Tao but ve
	penRED = CreatePen(PS_SOLID, 4, RGB(255, 0, 0));
	penBLUE = CreatePen(PS_SOLID, 4, RGB(0, 0, 255));
	penDEFAULT = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
	//
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	//
	HDC hdc;
	PAINTSTRUCT ps;
	RECT rect;
	POINT pt;
	switch (message)
	{
	case WM_CREATE:
		return 0;
	case WM_SIZE:
		GetClientRect(hwnd, &rect);
		ScreenWidth = rect.right - rect.left;
		CellWidth = ScreenWidth / M;
		ScreenHeight = rect.bottom - rect.top;
		CellHeight = ScreenHeight / N;
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		// Ve ban co
		SelectObject(hdc, penDEFAULT);
		for (int i = 0; i < M; i++)
		{
			DrawLine(hdc, i*CellWidth, 0, i*CellWidth, ScreenHeight); // Ve duong ke doc

		}
		for (int j = 0; j < N; j++)
		{
			DrawLine(hdc, 0, j*CellHeight, ScreenWidth, j*CellHeight); // Ve duong ke ngang
		}
		// Ve X hoac O
		for (int i = 0; i < M; i++)
		{
			for (int j = 0; j < N; j++)
			{
				if (map[i][j] == 1)
				{
					SelectObject(hdc, penBLUE);
					float left, top, right, bottom;
					left = i*CellWidth;
					top = j*CellHeight;
					right = left + CellWidth;
					bottom = top + CellHeight;
					Ellipse(hdc, left, top, right, bottom);

				}
				if (map[i][j] == -1)
				{
					SelectObject(hdc, penRED);
					DrawX(hdc, i*CellWidth, j*CellHeight);

				}

			}
		}
		// Dua ra thong bao neu co nguoi choi chien thang
		if (Winner != 0)
		{
			MessageBox(hwnd, (Winner == 1) ? (LPCSTR)"O WIN!" : (LPCSTR)"X WIN!", (LPCSTR)"Tro choi ket thuc", MB_OK);
			// Lam sach ban co
			for (int i = 0; i < M; i++)
			{
				for (int j = 0; j < N; j++)
				{
					map[i][j] = 0;
				}
			}
			Winner = 0;
			InvalidateRect(hwnd, NULL, true);
		}
		EndPaint(hwnd, &ps);
		UpdateWindow(hwnd);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_LBUTTONDOWN:
		int i, j;
		float tdx, tdy;
		tdx = LOWORD(lParam); // Toa do x cua con tro
		tdy = HIWORD(lParam); // Toa do y cua con tro
		i = (int)(tdx / CellWidth);
		j = (int)(tdy / CellHeight);

		if (map[i][j] == 0)
		{
			map[i][j] = status;
			status = status*-1;
		}

		Winner = Win(i, j, status*-1);

		InvalidateRect(hwnd, NULL, true);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

int DrawLine(HDC hdc, int xStart, int yStart, int xEnd, int yEnd) // Ham ve doan thang
{

	MoveToEx(hdc, xStart, yStart, NULL);
	LineTo(hdc, xEnd, yEnd);
	return 0;
}

int DrawX(HDC hdc, int x, int y)
{
	DrawLine(hdc, x, y, x + CellWidth, y + CellHeight);
	DrawLine(hdc, x, y + CellHeight, x + CellWidth, y);
	return 0;
}

// ham kiem tra chien thang tro choi
int Win(int x, int y, int person)
{
	int count;
	int i, j;
	// ngang
	count = 0;
	i = x;
	j = y;
	try{
		while (map[i][j] == person && i >= 0)
		{
			i--;
			count++;
		}
		i = x + 1;
		while (map[i][j] == person && i < M)
		{
			i++;
			count++;
		}
	}
	catch (exception& e) {}
	if (count >= 5) return person;
	// Doc
	count = 0;
	i = x;
	j = y;
	try{
		while (map[i][j] == person && j >= 0)
		{
			j--;
			count++;
		}
		j = y + 1;
		while (map[i][j] == person && j < N)
		{
			j++;
			count++;
		}
	}
	catch (exception& e) {}
	if (count >= 5) return person;
	// Xien tren -> duoi
	count = 0;
	i = x;
	j = y;
	try{
		while (map[i][j] == person && j >= 0 && i >= 0)
		{
			i--;
			j--;
			count++;
		}
		j = y + 1;
		i = x + 1;
		while (map[i][j] == person && j < N && i < M)
		{
			i++;
			j++;
			count++;
		}
	}
	catch (exception& e) {}
	if (count >= 5)	return person;
	// Xien duoi len
	count = 0;
	i = x;
	j = y;
	try{
		while (map[i][j] == person && j >= 0 && i < M)
		{
			i++;
			j--;
			count++;
		}
		j = y + 1;
		i = x - 1;
		while (map[i][j] == person && j < N && i >= 0)
		{
			i--;
			j++;
			count++;
		}
	}
	catch (exception& e) {}
	if (count >= 5)	return person;
	return 0;
}