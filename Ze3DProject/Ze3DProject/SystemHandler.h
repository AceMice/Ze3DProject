#ifndef SYSTEMHANDLER_H
#define SYSTEMHANDLER_H

#include "GraphicsHandler.h"
#include "InputHandler.h"

class SystemHandler {
private:
	HWND hwnd;
	HINSTANCE hinstance;
	LPCWSTR applicationName;
	GraphicsHandler* graphicsH;
	InputHandler* inputH;
	void InitWindow(int& screenWidth, int& screenHeight);
	void ShutdownWindow();
	bool Frame(float);
public:
	SystemHandler();
	~SystemHandler();

	bool Initialize();
	void Run();
	void Shutdown();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);
};
//FUNCTION PROTOTYPE
static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
static SystemHandler* ApplicationHandle = nullptr;

#endif