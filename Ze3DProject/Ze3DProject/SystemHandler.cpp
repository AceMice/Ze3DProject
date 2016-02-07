#include "SystemHandler.h"
SystemHandler::SystemHandler() {
	//this->InputObject = nullptr;
	this->graphicsH = nullptr;
}

SystemHandler::~SystemHandler() {
	//Cleanup is in ShutDown(), since in some instances, window functions never call the destructor 
}

void SystemHandler::InitWindow(int screenWidth, int screenHeight) {
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX;
	int posY;

	//Get an external pointer to this object
	ApplicationHandle = this;

	//Get the instance if this application
	this->hinstance = GetModuleHandle(NULL);

	//Give the application a name
	this->applicationName = L"Ze3DProjectEngine";

	//Setup the window class with defualt values
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = this->hinstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = nullptr;
	wc.lpszMenuName = this->applicationName;
	wc.cbSize = sizeof(WNDCLASSEX);

	//Register the window class
	RegisterClassEx(&wc);

	//Determine the resolution of the client desktop screen
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	//Setup the screen settings depending on whatever 
	//it is running in full screen or in windowed mode
	if (FULL_SCREEN){
		// If full screen, set the screen to maximum size of the user desktop and 32 bit
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		//Change the display settings to full screen
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		//Set the pposition of the window to the top left corner
		posX = posY = 0;
	
	}
	else {
		//If windowed then set it to 800x600
		screenWidth = 800;
		screenHeight = 600;

		//Place the window in the middle of the screen
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	//Create the window with the screen settings and get the handle to it
	this->hwnd = CreateWindowEx(
		WS_EX_APPWINDOW,
		this->applicationName,
		this->applicationName,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
		posX, posY, screenWidth, screenHeight,
		NULL, NULL,
		this->hinstance,
		NULL);

	// Bring the window up on the screen and set it as main focus
	ShowWindow(this->hwnd, SW_SHOW);
	SetForegroundWindow(this->hwnd);
	SetFocus(this->hwnd);

	//Hide/show the mouse cursor.
	ShowCursor(true);

	return;
}

bool SystemHandler::Frame() {
	bool result;

	//Cheeck if the user pressed escape and wants to exit the application
	//INPUT OBJECT NOT IMPLEMENTED YET

	//Do the frame processing for the graphics object
	result = this->graphicsH->Frame();
	if (!result) {
		return false;
	}

	return true;
}

void SystemHandler::ShutdownWindow() {
	//Show the mouse cursor.
	ShowCursor(true);

	//Fix the display settings if leaving full screen mode
	if (FULL_SCREEN) {
		ChangeDisplaySettings(NULL, 0);
	}

	//Remove the window
	DestroyWindow(this->hwnd);	//Expects this to clear memory
	this->hwnd = nullptr;

	//Remove the application instance
	UnregisterClass(this->applicationName, this->hinstance);
	this->hinstance = nullptr;

	//Release the pointer to this class
	ApplicationHandle = nullptr;

	return;
}


bool SystemHandler::Initialize() {
	int screenWidth;
	int screenHeight;
	bool result;

	//Initilize the window api
	InitWindow(screenWidth, screenHeight);

	//Create the input object. This object wil handle reading the keyboard input from the user
	//THIS HAS NOT BEEN IMPLEMENTED YET

	//Create the graphics object. This object will handle rendering all the graphics for this applicaion
	this->graphicsH = new GraphicsHandler;
	if (!this->graphicsH) {
		MessageBox(this->hwnd, L"new GraphicsHandler",L"Error",MB_OK);
		return false;
	}

	//Initialize the graphics object
	result = this->graphicsH->Initialize(screenWidth, screenHeight, this->hwnd);
	if (!result) {
		return false;
	}
	
	return true;

}

void SystemHandler::Run() {
	MSG msg;
	bool done = false;
	bool result;

	//Initialize the message structure
	ZeroMemory(&msg, sizeof(MSG));

	//Loop until there is quit message from the window or the user
	while (!done) {
		
		//Handle the window messages
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		//If windows signals to end the application then exit out
		if (msg.message == WM_QUIT) {
			done = true;
		}
		else {
			//Otherwise do the frame processing
			result = Frame();
			if(!result){
				done = true;
			}
		}
	}

	return;
}

void SystemHandler::Shutdown() {
	//Release the grpahics object
	if (this->graphicsH) {
		this->graphicsH->Shutdown();
		delete this->graphicsH;
		this->graphicsH = nullptr;
	}
	
	//Release the input object
	//NOT YET IMPLEMENTED

	return;
}

LRESULT CALLBACK SystemHandler::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam) {
	switch (umsg)
	{
		//Check if a key is pressed on the keyboard
		//Check if a key is released on the keyboard
		//INPUT OBJECT NOT IMPLEMENTED YET
		defualt:
		{
			return DefWindowProc(hwnd, umsg, wparam, lparam);
		}
	}
}

//GLOBAL FUNCTIONS
static LRESULT CALLBACK WndProc(HWND hwnd, UINT unmessage, WPARAM wparam, LPARAM lparam) {
	switch (unmessage)
	{
		//Check if the window is being destroyed
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}

		//Check if the window is being closed
		case WM_CLOSE:
		{
			PostQuitMessage(0);
			return 0;
		}

		//All other messages pass to the message handler in the system class
		defualt:
		{
			return ApplicationHandle->MessageHandler(hwnd, unmessage, wparam, lparam);
		}

	}
}
