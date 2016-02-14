#include "GraphicsHandler.h"

GraphicsHandler::GraphicsHandler()
{
	this->direct3DH = nullptr;
	this->modelH = nullptr;
	this->cameraH = nullptr;
	this->shaderH = nullptr;
}

GraphicsHandler::~GraphicsHandler()
{
}

bool GraphicsHandler::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;

	this->direct3DH = new D3DHandler;
	if (!this->direct3DH) {
		return false;
	}

	result = this->direct3DH->Initialize(screenWidth, screenHeight, hwnd, VSYNC_ENABLED, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the direct3d object!", L"Error", MB_OK);
		return false;
	}

	//Create the camera object
	this->cameraH = new CameraHandler;
	if (!this->cameraH)
	{
		return false;
	}

	//Set the initial position of the camera
	this->cameraH->SetPosition(0.0f, 0.0f, -5.0f);

	// Create the model object.
	this->modelH = new ModelHandler;
	if (!this->modelH)
	{
		return false;
	}

	//Initialize the model object
	result = this->modelH->Initialize(this->direct3DH->GetDevice(), this->direct3DH->GetDeviceContext(), "../Ze3DProject/Textures/stone1.tga");
	if (!result)
	{
		MessageBox(hwnd, L"this->modelH->Initialize", L"Error", MB_OK);
		return false;
	}

	// Create the color shader object.
	this->shaderH = new ShaderHandler;
	if (!this->shaderH)
	{
		return false;
	}

	// Initialize the color shader object.
	result = this->shaderH->Initialize(this->direct3DH->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"this->shaderH->Initialize", L"Error", MB_OK);
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
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	bool result;

	//Clear the buffers to begin the scene
	this->direct3DH->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	//Generate the view matrix based on the camera's position
	this->cameraH->Render();

	//Get the world, view, and projection matrices from the camera and d3d objects
	this->direct3DH->GetWorldMatrix(worldMatrix);
	this->cameraH->GetViewMatrix(viewMatrix);
	this->direct3DH->GetProjectionMatrix(projectionMatrix);

	//Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing
	this->modelH->Render(this->direct3DH->GetDeviceContext());

	//Render the model using the color shader
	result = this->shaderH->Render(this->direct3DH->GetDeviceContext(), this->modelH->GetIndexCount(), 
									worldMatrix, viewMatrix, projectionMatrix, this->modelH->GetTexture());
	if (!result)
	{
		return false;
	}

	//Display the rendered scene to screen
	this->direct3DH->EndScene();

	return true;
}

void GraphicsHandler::Shutdown()
{
	//Release the shaderHandler object
	if (this->shaderH)
	{
		this->shaderH->Shutdown();
		delete this->shaderH;
		this->shaderH = 0;
	}

	//Release the modelHandler object
	if (this->modelH)
	{
		this->modelH->Shutdown();
		delete this->modelH;
		this->modelH = 0;
	}

	//Release the cameraHandler object
	if (this->cameraH)
	{
		delete this->cameraH;
		this->cameraH = 0;
	}

	//Release the direct
	if (this->direct3DH) {
		this->direct3DH->Shutdown();
		delete this->direct3DH;
		this->direct3DH = nullptr;
	}
}