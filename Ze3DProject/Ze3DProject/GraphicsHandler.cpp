#include "GraphicsHandler.h"

GraphicsHandler::GraphicsHandler()
{
	this->direct3D = nullptr;
}

GraphicsHandler::~GraphicsHandler()
{
}

bool GraphicsHandler::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;

	this->direct3D = new D3DHandler;
	if (!this->direct3D) {
		return false;
	}

	result = this->direct3D->Initialize(screenWidth, screenHeight, hwnd, VSYNC_ENABLED, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the direct3d object!", L"Error", MB_OK);
		return false;
	}

	return true;
}

bool GraphicsHandler::Frame()
{
	bool result;

	result = this->Render();
	if (!result) {
		return false;
	}

	return true;
}

bool GraphicsHandler::Render()
{
	//Clear the buffers to begin the scene
	this->direct3D->BeginScene(0.5f, 0.5f, 0.5f, 1.0f);

	//Display the rendered scene to screen
	this->direct3D->EndScene();

	return true;
}

void GraphicsHandler::Shutdown()
{
	if (this->direct3D) {
		this->direct3D->Shutdown();
		delete this->direct3D;
		this->direct3D = nullptr;
	}
}