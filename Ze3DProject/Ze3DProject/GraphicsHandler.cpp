#include "GraphicsHandler.h"

GraphicsHandler::GraphicsHandler()
{
	this->direct3DH = nullptr;
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
	Model* tempModel = nullptr;

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
	tempModel = new Model;
	if (!tempModel)
	{
		return false;
	}

	//Initialize the ground object
	result = tempModel->Initialize(this->direct3DH->GetDevice(), this->direct3DH->GetDeviceContext(), "ground");
	if (!result)
	{
		MessageBox(hwnd, L"this->ground->Initialize", L"Error", MB_OK);
		return false;
	}
	//Insert model into vector
	this->models.push_back(tempModel);

	// Create the model2 object.
	tempModel = new Model;
	if (!tempModel)
	{
		return false;
	}
	//Initialize the OgreFullG object
	result = tempModel->Initialize(this->direct3DH->GetDevice(), this->direct3DH->GetDeviceContext(), "OgreFullG");
	if (!result)
	{
		MessageBox(hwnd, L"this->OgreFullG->Initialize", L"Error", MB_OK);
		return false;
	}
	//Insert model into vector
	this->models.push_back(tempModel);

	// Create the carSLS3 object.
	tempModel = new Model;
	if (!tempModel)
	{
		return false;
	}
	//Initialize the carSLS3 object
	result = tempModel->Initialize(this->direct3DH->GetDevice(), this->direct3DH->GetDeviceContext(), "carSLS3");
	if (!result)
	{
		MessageBox(hwnd, L"this->carSLS3->Initialize", L"Error", MB_OK);
		return false;
	}
	//Insert model into vector
	this->models.push_back(tempModel);

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
	Model* tempModel = nullptr;
	
	this->rotY += dTime / 800000;

	tempModel = this->GetModel("carSLS3");
	if (tempModel) {
		modelWorld = XMMatrixScaling(5.0f, 5.0f, 5.0f);
		modelWorld = XMMatrixRotationY(3.0f) * modelWorld;
		modelWorld = XMMatrixTranslation(-1.5f, -0.33f, 1.0f) * modelWorld;
		tempModel->SetWorldMatrix(modelWorld);
	}
	
	tempModel = this->GetModel("OgreFullG");
	if (tempModel) {
		modelWorld = XMMatrixScaling(0.7f, 0.7f, 0.7f);
		modelWorld = XMMatrixRotationY(2.0f) * modelWorld;
		modelWorld = XMMatrixTranslation(6.0f, -5.75f, -3.0f) * modelWorld;
		tempModel->SetWorldMatrix(modelWorld);
	}
	
	tempModel = this->GetModel("ground");
	if (tempModel) {
		modelWorld = XMMatrixTranslation(0.0f, -4.0f, 0.0f);
		tempModel->SetWorldMatrix(modelWorld);
	}

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
	int normMapIndex;
	XMFLOAT4 difColor;
	XMFLOAT4 specColor;
	bool transparent;


	//Clear the buffers to begin the scene
	this->direct3DH->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	//Get the view, and projection matrices from the camera and d3d objects
	this->cameraH->GetViewMatrix(viewMatrix);
	this->direct3DH->GetProjectionMatrix(projectionMatrix);

	//**NON TRANSPARENT RENDER**\\

	for (int i = 0; i < this->models.size(); i++) {

		//Get the world matrix from model
		this->models.at(i)->GetWorldMatrix(worldMatrix);

		//Put the model1 vertex and index buffers on the graphics pipeline to prepare them for drawing
		this->models.at(i)->Render(this->direct3DH->GetDeviceContext());

		//Draw all non transparent subsets
		modelSubsets = this->models.at(i)->NrOfSubsets();
		for (int j= 0; j < modelSubsets; j++) { 
			//Get all the nessecary information from the model
			this->models.at(i)->GetSubsetInfo(j, indexStart, indexCount, textureIndex, normMapIndex, difColor, specColor, transparent);

			//Render the model using the shader-handler
			if (!transparent) {
				result = this->shaderH->Render(this->direct3DH->GetDeviceContext(), indexCount, indexStart,
					worldMatrix, viewMatrix, projectionMatrix, this->models.at(i)->GetTexture(textureIndex), 
					this->models.at(i)->GetTexture(normMapIndex), difColor, specColor, false, this->cameraH->GetPosition());
				if (!result)
				{
					return false;
				}

			}
		}
	}

	//**TRANSPARENT RENDER**\\

	for (int i = 0; i < this->models.size(); i++) {

		//Get the world matrix from model
		this->models.at(i)->GetWorldMatrix(worldMatrix);

		//Put the model1 vertex and index buffers on the graphics pipeline to prepare them for drawing
		this->models.at(i)->Render(this->direct3DH->GetDeviceContext());

		//Draw all non transparent subsets
		modelSubsets = this->models.at(i)->NrOfSubsets();
		for (int j = 0; j < modelSubsets; j++) {

			//Get all the nessecary information from the model
			models.at(i)->GetSubsetInfo(j, indexStart, indexCount, textureIndex, normMapIndex, difColor, specColor, transparent);

			if (transparent) {
				//Render the model using the shader-handler
				result = this->shaderH->Render(this->direct3DH->GetDeviceContext(), indexCount, indexStart,
					worldMatrix, viewMatrix, projectionMatrix, this->models.at(i)->GetTexture(textureIndex),
					this->models.at(i)->GetTexture(normMapIndex), difColor, specColor, true, this->cameraH->GetPosition());
				if (!result)
				{
					return false;
				}
			}
		}
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

	//Release the Model objects
	for (int i = 0; i < this->models.size(); i++) {
		if (this->models.at(i)) {
			this->models.at(i)->Shutdown();
			delete this->models.at(i);
			this->models.at(i) = nullptr;
		}
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

Model* GraphicsHandler::GetModel(char* name)
{
	//Find the correct model by name
	for (int i = 0; i < this->models.size(); i++) {
		if (this->models.at(i)->GetName() == name) {
			return this->models.at(i);
		}
	}

	return NULL;
}