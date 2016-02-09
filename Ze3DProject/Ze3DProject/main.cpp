//FIRST!!

//#include <windows.h>
#include "SystemHandler.h"

#include <d3d11.h>
#include <d3dcompiler.h>

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	//MessageBox(NULL, L"TestBox", L"TestBox", MB_ICONEXCLAMATION | MB_OK);
	SystemHandler* sH;
	bool result;

	//Create the system object
	sH = new SystemHandler;

	if (!sH) {
		return 0;
	}

	//Initialize and run the system object
	result = sH->Initialize();
	if (result) {
		sH->Run();
	}

	//Shutdown and release the system object
	sH->Shutdown();
	delete sH;
	sH = nullptr;

	return 0;
}