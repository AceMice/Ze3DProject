#include "InputHandler.h"


InputHandler::InputHandler() {

}

InputHandler::InputHandler(const InputHandler&) {

}

InputHandler::~InputHandler() {

}

void InputHandler::Initialize(HINSTANCE hInstance, HWND hwnd, int screenWidth, int screenHeight) {
	HRESULT hr;

	this->mouseX = 0;
	this->mouseY = 0;
	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;

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

	hr = this->DIMouse->Acquire();
	if (FAILED(hr)) {
		MessageBox(hwnd, L"DIMouse->Acquire", L"Error", MB_OK);
	}

	return;
}

void InputHandler::Shutdown() {
	
	if (this->DIMouse) {
		this->DIMouse->Unacquire();
		this->DIMouse->Release();
		this->DIMouse = nullptr;
	}

	if (this->DirectInput) {
		this->DirectInput->Release();
		this->DirectInput = nullptr;
	}

	return;
}

bool InputHandler::Frame() {
	bool result;

	//Read the current state of the mouse
	result = this->readMouse();
	
	//Even if we cant read the mouse, we will still use the old information
	
	//Process the changes in the mouse
	this->ProcessInput();

	return true;
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

bool InputHandler::readMouse() {
	HRESULT hr;

	//Read the mouse device
	hr = this->DIMouse->GetDeviceState(sizeof(DIMOUSESTATE), &this->mouseState);
	if (FAILED(hr)) {
		
		//If the mouse lost focus or was not acquired, try to get control back
		if (hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED) {
			this->DIMouse->Acquire();
		}
		else {
			return false;
		}

	}

	return true;
}

void InputHandler::ProcessInput() {

	//Update the location of the mouse cursor based on the change of the mouse location during frame
	this->mouseX += this->mouseState.lX;
	this->mouseY += this->mouseState.lY;

	//Check if the mouse exits the screen
	if (this->mouseX < 0) {
		this->mouseX = 0;
	}
	if (this->mouseX > this->screenWidth) {
		this->mouseX = this->screenWidth;
	}
	if (this->mouseY < 0) {
		this->mouseY = 0;
	}
	if (this->mouseY > this->screenHeight) {
		this->mouseY = this->screenHeight;
	}

	return;
}

DirectX::XMVECTOR InputHandler::GetMouseDeltaPos() {
	
	DirectX::XMVECTOR result = DirectX::XMVectorSet(this->mouseState.lX, this->mouseState.lY,0,1);

	return result;
}