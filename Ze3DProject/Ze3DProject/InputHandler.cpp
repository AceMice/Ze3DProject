#include "InputHandler.h"


InputHandler::InputHandler() {

}

InputHandler::InputHandler(const InputHandler&) {

}

InputHandler::~InputHandler() {

}

void InputHandler::Initialize() {
	//Initialize all the keys to being released and not pressed
	for (int i = 0; i < 256; i++) {
		this->keys[i] = false;
	}

	return;
}

void InputHandler::KeyDown(unsigned int input) {
	//If a key is pressed then save that in the key array
	this->keys[input] = true;
	
	return;
}

void InputHandler::KeyUp(unsigned int input) {
	//If a key is released then clear the state in the key array
	this->keys[input] = false;
	return;
}

bool InputHandler::IsKeyDown(unsigned int key) {
	//Return what state the key is in
	return this->keys[key];
}

