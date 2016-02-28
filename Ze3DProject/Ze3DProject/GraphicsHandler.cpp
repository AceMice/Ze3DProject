#include "GraphicsHandler.h"

GraphicsHandler::GraphicsHandler()
{
	this->direct3DH = nullptr;
	this->model1 = nullptr;
	this->cameraH = nullptr;
	this->shaderH = nullptr;

	this->rotY = 0.0f;
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
	this->cameraH->SetPosition(0.0f, 0.0f, -20.0f);

	// Create the model1 object.
	this->model1 = new Model;
	if (!this->model1)
	{
		return false;
	}

	//Initialize the model1 object
	result = this->model1->Initialize(this->direct3DH->GetDevice(), this->direct3DH->GetDeviceContext(), "carSLS");
	if (!result)
	{
		MessageBox(hwnd, L"this->model1->Initialize", L"Error", MB_OK);
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

bool GraphicsHandler::Frame(float dTime, InputHandler* inputH)
{
	bool result;
	XMMATRIX modelWorld;
	
	/*this->rotY += dTime / 800000;
	modelWorld = XMMatrixRotationY(this->rotY);
	this->model1->SetWorldMatrix(modelWorld);*/

	modelWorld = XMMatrixRotationY(2.5f);
	modelWorld = XMMatrixScaling(0.6f, 0.6f, 0.6f) * modelWorld;
	modelWorld = XMMatrixTranslation(0.0f, -5.0f, -3.0f) * modelWorld;
	this->model1->SetWorldMatrix(modelWorld);

	//Generate the view matrix based on the camera's position
	this->cameraH->Frame(dTime, inputH);

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
	int indexCount;
	int indexStart;
	int modelSubsets;
	int textureIndex;
	XMFLOAT4 color;

	//Clear the buffers to begin the scene
	this->direct3DH->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	//Get the view, and projection matrices from the camera and d3d objects
	this->cameraH->GetViewMatrix(viewMatrix);
	this->direct3DH->GetProjectionMatrix(projectionMatrix);

	//Get the world matrix from model1
	this->model1->GetWorldMatrix(worldMatrix);

	//Put the model1 vertex and index buffers on the graphics pipeline to prepare them for drawing
	this->model1->Render(this->direct3DH->GetDeviceContext());

	modelSubsets = this->model1->NrOfSubsets();
	for (int i = 0; i < modelSubsets; i++) {
		model1->GetSubsetInfo(i, indexStart, indexCount, textureIndex, color);


		//Render the model1 using the color shader
		result = this->shaderH->Render(this->direct3DH->GetDeviceContext(), indexCount, indexStart,
			worldMatrix, viewMatrix, projectionMatrix, this->model1->GetTexture(textureIndex), color);
		if (!result)
		{
			return false;
		}
	}

	////Render the model1 using the color shader
	//result = this->shaderH->Render(this->direct3DH->GetDeviceContext(), this->model1->GetIndexCount(), 
	//							worldMatrix, viewMatrix, projectionMatrix, this->model1->GetTexture(), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
	//if (!result)
	//{
	//	return false;
	//}

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

	//Release the Model1 object
	if (this->model1)
	{
		this->model1->Shutdown();
		delete this->model1;
		this->model1 = 0;
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