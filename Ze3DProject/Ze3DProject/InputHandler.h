#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H

#pragma comment (lib, "dinput8.lib")
#pragma comment (lib, "dxguid.lib")
#include <dinput.h>
#include <DirectXMath.h>

class InputHandler {
private:
	bool keys[256];
	IDirectInputDevice8* DIMouse;
	DIMOUSESTATE mouseState;
	LPDIRECTINPUT8 DirectInput;
	int mouseX;
	int mouseY;
	int screenWidth;
	int screenHeight;

public:
	InputHandler();
	InputHandler(const InputHandler&);
	~InputHandler();

	void Initialize(HINSTANCE, HWND, int, int);
	void Shutdown();
	bool Frame();
	void KeyDown(unsigned int);
	void KeyUp(unsigned int);
	bool IsKeyDown(unsigned int);
	bool readMouse();
	void ProcessInput();
	DirectX::XMVECTOR GetMouseDeltaPos();
};


#endif
