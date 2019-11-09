#include <windows.h>
#include <math.h>
#include <string>
#include <time.h>
#include <fstream>

/*
	2. Изучение вывода текста и шрифтов.

	Разработать программу, которая вписывает в окно текстовую таблицу (N строк на M столбцов)
	таким образом, что все столбцы таблицы равномерно
	распределяются по ширине окна, а высота строк таблицы подбирается таким образом,
	чтобы вместить текст каждой ячейки.
	Размерность таблицы задается перед рисовкой (консольно, например).
	При изменении размеров окна таблица перерасчитывается и перерисовывается. Можно скейлить шрифт.
	Текст загружается из любого текстового файла(для демонстрации хотя бы два). Пробелы убираются.
*/

#define DELTA 3
#define BOLD_MENU_ID 11
#define COURSIVE_MENU_ID 22
#define UNDERLINE_MENU_ID 33

using namespace std;

int rows;
int columns;
string **stringMatrix;
ifstream fileName("text.txt");

bool isBold = false;
bool isCoursive = false;
bool isUnderline = false;


HFONT generateFont()
{
	int fnWeight = 400;
	DWORD fdwItalic = FALSE;
	DWORD fdwUnderline = FALSE;

	if (isBold) fnWeight += 300;
	if (isCoursive) fdwItalic = TRUE;
	if (isUnderline) fdwUnderline = TRUE;

	DWORD fontFamily;
	switch (2)	//rand() % 2 + 1
	{
	case 1:
		fontFamily = FF_DECORATIVE;
		break;
	case 2:
		fontFamily = FF_DONTCARE;
		break;
	}
	return CreateFont(0, 0, 0, 0, fnWeight, fdwItalic, fdwUnderline, FALSE, FALSE,
		FALSE, FALSE, FALSE, DEFAULT_PITCH | fontFamily, NULL);
}


void Initialization(HDC hdc)
{

	columns = 5;
	rows = 10;

	stringMatrix = new string *[rows];
	for (int i = 0; i < rows; i++)
		stringMatrix[i] = new string[columns];

	string str;
	for (char c = fileName.get(); fileName.good(); c = fileName.get())
	{
		if (c != '\n') // && c != ' '
			str += c;
	}


	fileName.close();

	int len = str.length();

	int quantityOfCharInCell = (int)len / (columns * rows);
	int charactersLeft = len % (columns * rows);

	int currentOffset = 0;
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < columns; j++)
		{
			int countCharsInThisCell = quantityOfCharInCell;

			if (charactersLeft-- > 0)
				countCharsInThisCell++;

			stringMatrix[i][j] = str.substr(currentOffset, countCharsInThisCell);

			currentOffset += countCharsInThisCell;
		}
	}
}


string getLongestString(int currentLine)
{
	string longestSrting = stringMatrix[currentLine][0];
	for (int j = 1; j < columns; j++)
	{
		if (stringMatrix[currentLine][j].length() > longestSrting.length())
			longestSrting = stringMatrix[currentLine][j];
	}

	return longestSrting;
}


int GetBlockHeight(HDC hdc, int currentLine, int width)
{
	string longestString = getLongestString(currentLine);

	RECT nonDrawableBlock;
	nonDrawableBlock.left = 0;
	nonDrawableBlock.top = 0;
	nonDrawableBlock.bottom = 1;
	nonDrawableBlock.right = width;

	//draw longest string to deside block height
	return (int)DrawText(hdc, longestString.c_str(), longestString.length(), &nonDrawableBlock,
		DT_WORDBREAK | DT_CENTER | DT_CALCRECT | DT_EDITCONTROL) + DELTA;
}


void DrawTextBlock(HDC hdc, int left, int top, int width, int height, int raw, int column)
{
	RECT drawableBlock;
	drawableBlock.left = left;
	drawableBlock.top = top + DELTA;
	drawableBlock.bottom = top + height;
	drawableBlock.right = left + width - DELTA;

	DrawText(hdc, stringMatrix[raw][column].c_str(),
		stringMatrix[raw][column].length(),
		&drawableBlock,
		DT_WORDBREAK | DT_EDITCONTROL | DT_CENTER);
}


void DrawLine(HDC hdc, int x1, int y1, int x2, int y2)
{
	MoveToEx(hdc, x1, y1, NULL);
	LineTo(hdc, x2, y2);
}


void DrawTable(HDC hdc, int width, int height)
{
	int blockWidth = (int)width / columns;
	int currentTop = 0;
	HFONT newFont;

	for (int i = 0; i < rows; i++)
	{
		int blockHeigth = (int)GetBlockHeight(hdc, i, blockWidth + DELTA);

		for (int j = 0; j < columns; j++)
		{
			newFont = generateFont();
			SelectObject(hdc, newFont);

			DrawTextBlock(hdc, (int)j * blockWidth, currentTop, blockWidth, blockHeigth, i, j);
			DrawLine(hdc, (j + 1) * blockWidth - 2, currentTop, (j + 1) * blockWidth - 2, blockHeigth + currentTop); //vertical

			DeleteObject(newFont);
		}

		currentTop += blockHeigth; //moving down
		DrawLine(hdc, 0, currentTop, blockWidth * columns, currentTop); //horizontal
	}
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static int width = 0;
	static int height = 0;

	HDC hdc;
	PAINTSTRUCT ps;
	RECT window = {};

	switch (message)
	{
	case WM_SIZE:
		width = LOWORD(lParam);
		height = HIWORD(lParam);
		UpdateWindow(hWnd);
		InvalidateRect(hWnd, NULL, TRUE);
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		DrawTable(hdc, width, height);
		ReleaseDC(hWnd, hdc);
		EndPaint(hWnd, &ps);
		break;
	case WM_COMMAND:
		GetWindowRect(hWnd, &window);
		switch (LOWORD(wParam))
		{
		case BOLD_MENU_ID:
			isBold = !isBold;
			PostMessage(hWnd, WM_SIZE, 0, LOWORD(window.right - window.left) + HIWORD(window.bottom - window.top));
			PostMessage(hWnd, WM_PAINT, 0, LOWORD(window.right - window.left) + HIWORD(window.bottom - window.top));
			break;
		case COURSIVE_MENU_ID:
			isCoursive = !isCoursive;
			PostMessage(hWnd, WM_SIZE, 0, LOWORD(window.right - window.left) + HIWORD(window.bottom - window.top));
			PostMessage(hWnd, WM_PAINT, 0, LOWORD(window.right - window.left) + HIWORD(window.bottom - window.top));
			break;
		case UNDERLINE_MENU_ID:
			isUnderline = !isUnderline;
			PostMessage(hWnd, WM_SIZE, 0, LOWORD(window.right - window.left) + HIWORD(window.bottom - window.top));
			PostMessage(hWnd, WM_PAINT, 0, LOWORD(window.right - window.left) + HIWORD(window.bottom - window.top));
			break;
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		exit(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}


//params  application descriptor   //previous app descriptor   //cmd pointer   //window style
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEX wcex;
	HWND hWnd;
	MSG msg;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_DBLCLKS;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = NULL;
	wcex.hCursor = NULL;
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = 0;
	wcex.lpszClassName = "TableBuilderClass";
	wcex.hIconSm = wcex.hIcon;
	RegisterClassEx(&wcex);

	hWnd = CreateWindow("TableBuilderClass", "osisp_by_naty",
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0,
		CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	Initialization(GetDC(hWnd));

	HMENU menu = CreateMenu();
	AppendMenu(menu, 0, BOLD_MENU_ID, "BOLD");
	SetMenu(hWnd, menu);
	AppendMenu(menu, 0, COURSIVE_MENU_ID, "COURSIVE");
	SetMenu(hWnd, menu);
	AppendMenu(menu, 0, UNDERLINE_MENU_ID, "UNDERLINE");
	SetMenu(hWnd, menu);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	while (GetMessage(&msg, hWnd, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}