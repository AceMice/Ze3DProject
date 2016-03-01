//FIRST!!

//#include <windows.h>
#include "SystemHandler.h"

#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <crtdbg.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <iostream>

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")

static const WORD MAX_CONSOLE_LINES = 500;

#define _DEBUG

#ifdef _DEBUG

void RedirectIOToConsole()

{

	int hConHandle;

	long lStdHandle;

	CONSOLE_SCREEN_BUFFER_INFO coninfo;

	FILE *fp;

	// allocate a console for this app

	AllocConsole();

	// set the screen buffer to be big enough to let us scroll text

	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE),

		&coninfo);

	coninfo.dwSize.Y = MAX_CONSOLE_LINES;

	SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE),

		coninfo.dwSize);

	// redirect unbuffered STDOUT to the console

	lStdHandle = (long)GetStdHandle(STD_OUTPUT_HANDLE);

	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);

	fp = _fdopen(hConHandle, "w");

	*stdout = *fp;

	setvbuf(stdout, NULL, _IONBF, 0);

	// redirect unbuffered STDIN to the console

	lStdHandle = (long)GetStdHandle(STD_INPUT_HANDLE);

	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);

	fp = _fdopen(hConHandle, "r");

	*stdin = *fp;

	setvbuf(stdin, NULL, _IONBF, 0);

	// redirect unbuffered STDERR to the console

	lStdHandle = (long)GetStdHandle(STD_ERROR_HANDLE);

	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);

	fp = _fdopen(hConHandle, "w");

	*stderr = *fp;

	setvbuf(stderr, NULL, _IONBF, 0);

	// make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog

	// point to console as well

	ios::sync_with_stdio();

}

#endif


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	//RedirectIOToConsole();

	//std::cout << "Working...";

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