#include "InputHandler.h"


InputHandler::InputHandler() {

}

InputHandler::InputHandler(const InputHandler&) {

}

InputHandler::~InputHandler() {

}

void InputHandler::Initialize(HINSTANCE hInstance, HWND hwnd) {
	HRESULT hr;
	//Initialize all the keys to being released and not pressed
	for (int i = 0; i < 256; i++) {
		this->keys[i] = false;
	}

	hr = DirectInput8Create(hInstance,
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(void**)&this->DirectInput,
		NULL
		);
	if (FAILED(hr)) {
		MessageBox(hwnd, L"DirectInput8Create", L"Error", MB_OK);
	}

	hr = DirectInput->CreateDevice(GUID_SysMouse, &this->DIMouse, NULL);
	if (FAILED(hr)) {
		MessageBox(hwnd, L"DirectInput->CreateDevice", L"Error", MB_OK);
	}

	hr = this->DIMouse->SetDataFormat(&c_dfDIMouse);
	if (FAILED(hr)) {
		MessageBox(hwnd, L"DIMouse->SetDataFormat", L"Error", MB_OK);
	}

	hr = this->DIMouse->SetCooperativeLevel(hwnd, DISCL_EXCLUSIVE | DISCL_NOWINKEY | DISCL_FOREGROUND);
	if (FAILED(hr)) {
		MessageBox(hwnd, L"DIMouse->SetCooperativeLevel", L"Error", MB_OK);
	}


	this->DIMouse->GetDeviceState(sizeof(DIMOUSESTATE), &this->mouseLastState);

	return;
}

void InputHandler::Shutdown() {
	this->DIMouse->Unacquire();
	this->DirectInput->Release();
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

DIMOUSESTATE InputHandler::getOldMouseState() {
	return this->mouseLastState;
}


void InputHandler::getNewMouseState(DIMOUSESTATE &mouseCurrentState) {
	
	this->DIMouse->GetDeviceState(sizeof(DIMOUSESTATE), &mouseCurrentState);

}