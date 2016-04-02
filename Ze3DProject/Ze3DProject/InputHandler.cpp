#include "InputHandler.h"


InputHandler::InputHandler() {
	this->lastKeyPressed = -1;
	this->mouseX = 0;
	this->mouseY = 0;
	this->screenWidth = 0;
	this->screenHeight = 0;

}

InputHandler::InputHandler(const InputHandler&) {

}

InputHandler::~InputHandler() {

}

void InputHandler::Initialize(HINSTANCE hInstance, HWND hwnd, int screenWidth, int screenHeight) {
	HRESULT hr;
	SetPhysicalCursorPos((GetSystemMetrics(SM_CXSCREEN)) / 2, (GetSystemMetrics(SM_CYSCREEN)) / 2);
	this->mouseX = (GetSystemMetrics(SM_CXSCREEN)) / 2;
	this->mouseY = (GetSystemMetrics(SM_CYSCREEN)) / 2;
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

	hr = this->DIMouse->SetCooperativeLevel(hwnd, DISCL_NONEXCLUSIVE | DISCL_NOWINKEY | DISCL_FOREGROUND);	//MOUSE THING
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

bool InputHandler::Frame(bool windowFocus) {
	bool result;

	//Read the current state of the mouse
	result = this->readMouse();
	
	//Even if we cant read the mouse, we will still use the old information
	
	//Process the changes in the mouse
	this->ProcessInput(windowFocus);

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
	this->lastKeyPressed = input;

	return;
}

bool InputHandler::IsKeyDown(unsigned int key) {
	//Return what state the key is in
	return this->keys[key];
}

bool InputHandler::IsKeyReleased(unsigned int key) {
	if ( this->lastKeyPressed == key ) {
		this->lastKeyPressed = -1;
		return true;
	}
	
	return false;
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

void InputHandler::ProcessInput(bool windowFocus) {

	//Update the location of the mouse cursor based on the change of the mouse location during frame
	POINT tempP;
	GetCursorPos(&tempP);
	this->mouseX = tempP.x - (GetSystemMetrics(SM_CXSCREEN) - this->screenWidth) / 2;
	this->mouseY = tempP.y - (GetSystemMetrics(SM_CYSCREEN) - this->screenHeight) / 2;

	if (windowFocus) {
		//Check if the mouse exits the screen
		if (this->mouseX < 0) {
			SetPhysicalCursorPos((GetSystemMetrics(SM_CXSCREEN) - this->screenWidth) / 2, tempP.y);
		}
		if (this->mouseX > this->screenWidth) {
			SetPhysicalCursorPos(((GetSystemMetrics(SM_CXSCREEN) - this->screenWidth) / 2) + this->screenWidth, tempP.y);
		}
		if (this->mouseY < 0) {
			SetPhysicalCursorPos(tempP.x, (GetSystemMetrics(SM_CYSCREEN) - this->screenHeight) / 2);
		}
		if (this->mouseY > this->screenHeight) {
			SetPhysicalCursorPos(tempP.x, ((GetSystemMetrics(SM_CYSCREEN) - this->screenHeight) / 2) + this->screenHeight);
		}
	}
	

	return;
}

DirectX::XMVECTOR InputHandler::GetMouseDeltaPos() {

	return DirectX::XMVectorSet(this->mouseState.lX, this->mouseState.lY, 0, 0);	//z,y is not used so set to 0;
}

DirectX::XMVECTOR InputHandler::GetMouseViewPos(DirectX::XMMATRIX projM) {
	DirectX::XMFLOAT4X4 tempM;
	DirectX::XMStoreFloat4x4(&tempM,projM);
	
	float viewSpaceX = ((2 * this->mouseX) / this->screenWidth) -1;
	float viewSpaceY = ((-2 * this->mouseY) / this->screenHeight) +1;
	float viewSpaceZ = 1.f;

	viewSpaceX = viewSpaceX / tempM._11;
	viewSpaceY = viewSpaceY / tempM._22;

	DirectX::XMVECTOR result = DirectX::XMVectorSet(viewSpaceX, viewSpaceY, viewSpaceZ, 0);
	
	return result;
}