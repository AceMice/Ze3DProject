#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H

#pragma comment (lib, "dinput8.lib")
#pragma comment (lib, "dxguid.lib")
#include <windows.h>
#include <dinput.h>
#include <DirectXMath.h>

class InputHandler {
private:
	bool keys[256];
	IDirectInputDevice8* DIMouse;
	DIMOUSESTATE mouseState;
	LPDIRECTINPUT8 DirectInput;
	float mouseX;
	float mouseY;
	int screenWidth;
	int screenHeight;
	unsigned int lastKeyPressed;

public:
	InputHandler();
	InputHandler(const InputHandler&);
	~InputHandler();

	void Initialize(HINSTANCE, HWND, int, int);
	void Shutdown();
	bool Frame(bool windowFocus);
	void KeyDown(unsigned int);
	void KeyUp(unsigned int);
	bool IsKeyDown(unsigned int);
	bool IsKeyReleased(unsigned int);
	bool readMouse();
	void ProcessInput(bool windowFocus);
	DirectX::XMVECTOR GetMouseDeltaPos();
	DirectX::XMVECTOR GetMouseViewPos(DirectX::XMMATRIX);
};


#endif
