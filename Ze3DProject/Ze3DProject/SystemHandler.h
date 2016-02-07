#ifndef SYSTEMHANDLER_H
#define SYSTEMHANDLER_H

#include "GraphicsHandler.h"

class SystemHandler {
private:
	HWND hwnd;
	HINSTANCE hinstance;
	LPCWSTR applicationName;

	void InitWindow(int screenWidth, int screenHeight);
	void ShutdownWindow();
public:
	SystemHandler();
	~SystemHandler();

	bool Initialize();
	void Run();
	void Shutdown();

	LRESULT CALLBACK MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam);
};

#endif