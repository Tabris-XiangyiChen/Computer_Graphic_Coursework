#pragma once

#define NOMIMAX
#define WINDOW_GET_X_LPARAM(lp) ((int)(short)LOWORD(lp))
#define WINDOW_GET_Y_LPARAM(lp) ((int)(short)HIWORD(lp))

#include <Windows.h>
#include <string>

class Window
{
public:
	HWND hwnd;
	HINSTANCE hinstance;
	std::string name;
	unsigned int width;
	unsigned int height;

	//key presses and mouse position
	bool keys[256];
	int mousex;
	int mousey;
	bool mouseButtons[3];


	Window() {}

	void create(std::string _window_name, unsigned int _window_width, unsigned int _window_height, int _window_x, int _window_y);

	void updateMouse(int x, int y)
	{
		mousex = x;
		mousey = y;
		
	}

	void processMessages();


};