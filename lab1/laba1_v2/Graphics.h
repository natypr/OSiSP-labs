#pragma once
#include <Windows.h>

struct vergePoint
{
	float top, bottom, left, right, mouseX, mouseY;
	bool initialized;
};

class Graphics
{
	HDC device;
	HDC bufferDevice;
	HBITMAP hScreen;
	HBITMAP oldBmp;
	RECT workSpace;
	PAINTSTRUCT PStruct;
	vergePoint points;

	BITMAP bm; 
	HDC spriteDevice;
	HBITMAP image;

public:
	Graphics(HWND hwnd, RECT workSpace);
	~Graphics();

	void DrawEllipse(HWND hWnd, float X, float Y, float radius);
	void DrawSprite(HWND hWnd, float X, float Y);
	vergePoint *GetPoints(int type, float radius);

	void DrawBackground(HWND hwnd) {
		HDC hdc = GetDC(hwnd);
		HDC memory = CreateCompatibleDC(hdc);

		HBRUSH brush = CreateSolidBrush(RGB(255, 0, 0));
		RECT rect;
		GetWindowRect(hwnd, &rect);
		float width = rect.right - rect.left;
			float height = rect.bottom - rect.top;
		rect.left = 0;
		rect.right = width;
		rect.top = 0;
		rect.bottom = height;
		FillRect(hdc, &rect, brush);
	}
};