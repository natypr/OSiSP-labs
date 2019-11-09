#include "Graphics.h"	//red
#include <Windows.h>
#include <cmath>

Graphics *gfx; 
vergePoint *vergePoints;
RECT workSpace = { 0, 0, 700, 500 };

float X = 0.0f;
float Y = 0.0f;
float radius = 50.0f;

float SPEED = 0.5f;
float WHEELSPEED = 10.f;
float JMP = 20.f;

/*1. �������� ���������� ����������� Windows-����������, 
��������� ��������� ���������, ��������� ����������� ����.

- ����������� ���������, ����������� ����������� � ������� ���������� � ���� ������ 
(���������� ������������� ��� ������) ������ ������� ������� ����.
- ���������� ������ �������� ����. ������������� ������� ������ �� ���������. 
� ������������ �������� Shift ������������� �������� ������� ������ �� �����������.
- �������� ������ �� �������� � ��������������� ��������.
- ������� ������� �������� � �������� �� ������ ����.
*/

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//�������� �� ������� �������
	if (GetAsyncKeyState(VK_UP))
		Y -= SPEED;
	if (GetAsyncKeyState(VK_DOWN))
		Y += SPEED;
	if (GetAsyncKeyState(VK_LEFT))
		X -= SPEED;
	if (GetAsyncKeyState(VK_RIGHT))
		X += SPEED;
	
	
	if (uMsg == WM_MOUSEWHEEL && GET_KEYSTATE_WPARAM(wParam) == MK_SHIFT)
	{
		if (GET_WHEEL_DELTA_WPARAM(wParam) > 0)
			X += WHEELSPEED;
		else
			X -= WHEELSPEED;
	}
	else if (uMsg == WM_MOUSEWHEEL && GET_KEYSTATE_WPARAM(wParam) != MK_SHIFT)
	{
		if (GET_WHEEL_DELTA_WPARAM(wParam) > 0)
			Y -= WHEELSPEED;
		else
			Y += WHEELSPEED;
	}


	if (uMsg == WM_DESTROY)
	{
		//����� ������ ������ �� ��, ��� ����� �����
		PostQuitMessage(0);
		return 0;
	}

	if (uMsg == WM_PAINT)
	{
		//gfx->DrawEllipse(hwnd, X, Y, radius);
//		gfx->DrawBackground(hwnd);
		gfx->DrawSprite(hwnd, X, Y);
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}


//HINSTANCE ������������� ����������, HWND ������������� ����
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, LPSTR cmdLine, int nCmdShow)
{
	WNDCLASSEX windowClass;
	MSG message;
	POINT mouseCoords;
	float distance = 0.0f;
		
	ZeroMemory(&windowClass, sizeof(WNDCLASSEX));
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.hbrBackground = (HBRUSH)COLOR_WINDOW+1;
//	windowClass.hbrBackground = (HBRUSH)(CreateSolidBrush(RGB(255,0,0)));
	windowClass.hInstance = hInstance;
	windowClass.lpfnWndProc = WindowProc;
	windowClass.lpszClassName = "MainWindow";
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClassEx(&windowClass);


/*	case WM_GETMINMAXINFO:
	{
		LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
		lpMMI->ptMinTrackSize.x = 500;
		lpMMI->ptMinTrackSize.y = 400;

		lpMMI->ptMaxTrackSize.x = 500;
		lpMMI->ptMaxTrackSize.y = 400;
	}
	break;
	*/


	// ��������� ����������� ������� ���� � ��������������� C������
	AdjustWindowRectEx(&workSpace, WS_OVERLAPPEDWINDOW, NULL, WS_EX_OVERLAPPEDWINDOW);

	HWND winHandle = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, "MainWindow", "Moving Figure", WS_OVERLAPPEDWINDOW,
		220, 70, workSpace.right - workSpace.left, workSpace.bottom - workSpace.top, NULL, NULL, hInstance, 0);

	if (!winHandle) return -1;

	
	X = (workSpace.right - workSpace.left) / 2.5f;
	Y = (workSpace.bottom - workSpace.top) / 2.5f;

	message.message = WM_NULL;

	gfx = new Graphics(winHandle, workSpace);
	if (!gfx) return -1;
	vergePoints = gfx->GetPoints(1, radius);

	ShowWindow(winHandle, nCmdShow);

	while (message.message != WM_QUIT)
	{
		//��������� ���������� ������� ������� ����
		GetClientRect(winHandle, &workSpace);

		//���� �� � ������� ��������� -> ���������� ��������� ������� ���������
		if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
		{
			DispatchMessage(&message);
		}

		//� ����� ��������� ��������� ������� (���� ��������)
		if (GetAsyncKeyState(VK_RBUTTON))
		{
			GetCursorPos(&mouseCoords);
			//������������ �������� ���������� ��������� ����� �� ������ � ���������� ������� �������
			ScreenToClient(winHandle, &mouseCoords);

			mouseCoords.x += vergePoints->mouseX; 
			mouseCoords.y += vergePoints->mouseY;

			distance = sqrt((mouseCoords.x - X) * (mouseCoords.x - X) + (mouseCoords.y - Y) * (mouseCoords.y - Y));

			X += SPEED * (mouseCoords.x - X) / distance;
			Y += SPEED * (mouseCoords.y - Y) / distance;

			if (distance < 0.5f)
			{
				X = mouseCoords.x;
				Y = mouseCoords.y;
			}
		}		

		if (Y - vergePoints->top <= workSpace.top) {
			Y = workSpace.top + vergePoints->top;
			InvalidateRect(winHandle, NULL, FALSE);
			Y += JMP;
		}
		if (X + vergePoints->right >= workSpace.right) {
			X = workSpace.right - vergePoints->right;
			InvalidateRect(winHandle, NULL, FALSE);
			X -= JMP;
		}
		if (Y + vergePoints->bottom >= workSpace.bottom) {
			Y = workSpace.bottom - vergePoints->bottom;
			InvalidateRect(winHandle, NULL, FALSE);
			Y -= JMP;
		}
		if (X - vergePoints->left <= workSpace.left) {
			X = workSpace.left + vergePoints->left;
			InvalidateRect(winHandle, NULL, FALSE);
			X += JMP;
		}

		InvalidateRect(winHandle, NULL, FALSE);
	}
	delete gfx;

	return 0;
}