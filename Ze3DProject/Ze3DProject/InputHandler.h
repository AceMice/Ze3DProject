#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H

class InputHandler {
private:
	bool keys[256];

public:
	InputHandler();
	InputHandler(const InputHandler&);
	~InputHandler();

	void Initialize();
	void KeyDown(unsigned int);
	void KeyUp(unsigned int);
	bool IsKeyDown(unsigned int);

};


#endif
