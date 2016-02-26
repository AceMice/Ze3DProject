#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H

#include <dinput.h>

class InputHandler {
private:
	bool keys[256];
	IDirectInputDevice8* DIMouse;
	DIMOUSESTATE mouseLastState;
	LPDIRECTINPUT8 DirectInput;

public:
	InputHandler();
	InputHandler(const InputHandler&);
	~InputHandler();

	void Initialize(HINSTANCE, HWND);
	void Shutdown();
	void KeyDown(unsigned int);
	void KeyUp(unsigned int);
	bool IsKeyDown(unsigned int);

};


#endif
