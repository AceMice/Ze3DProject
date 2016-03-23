#include "GraphicsHandler.h"

GraphicsHandler::GraphicsHandler()
{
	this->direct3DH = nullptr;
	this->cameraH = nullptr;
	this->shaderH = nullptr;
	this->colorShaderH = nullptr;
	this->modelWindow = nullptr;
	this->shadowShaderH = nullptr;

	this->rotY = 0.0f;
	this->moveLight = 0.0f;
	this->increase = true;
}

GraphicsHandler::~GraphicsHandler()
{
}

bool GraphicsHandler::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;
	Model* tempModel = nullptr;
	GroundModel* tempGroundModel = nullptr;

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
	this->cameraH->GenerateBaseViewMatrix();



	//Create the GroundModel Object
	tempGroundModel = new GroundModel;

	result = tempGroundModel->Initialize(this->direct3DH->GetDevice(), this->direct3DH->GetDeviceContext(), "BMPSmall");
	if (!result)
	{
		MessageBox(hwnd, L"this->groundModel->Initialize", L"Error", MB_OK);
		return false;
	}
	//Insert model into vector
	this->groundModels.push_back(tempGroundModel);
	tempGroundModel = nullptr;



	// Create the model1 object.
	tempModel = new Model;
	if (!tempModel)
	{
		return false;
	}

	//Initialize the standard ground object
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

	// Create the deferred shader object.
	this->shaderH = new ShaderHandler;
	if (!this->shaderH) {
		return false;
	}

	// Initialize the deferred shader object.
	result = this->shaderH->Initialize(this->direct3DH->GetDevice(), hwnd);
	if (!result) {
		MessageBox(hwnd, L"this->shaderH->Initialize", L"Error", MB_OK);
		return false;
	}

	// Create the shadow shader object.
	this->shadowShaderH = new ShadowShaderHandler;
	if (!this->shadowShaderH) {
		return false;
	}

	// Initialize the deferred shader object.
	result = this->shadowShaderH->Initialize(this->direct3DH->GetDevice(), hwnd);
	if (!result) {
		MessageBox(hwnd, L"this->shadowShaderH->Initialize", L"Error", MB_OK);
		return false;
	}


	// Create the color shader object.
	this->colorShaderH = new ColorShaderHandler;
	if (!this->colorShaderH) {
		return false;
	}

	// Initialize the color shader object.
	result = this->colorShaderH->Initialize(this->direct3DH->GetDevice(), hwnd);
	if (!result) {
		MessageBox(hwnd, L"this->colorShaderH->Initialize", L"Error", MB_OK);
		return false;
	}

	//Create the model for rendering 2d to
	this->modelWindow = new ModelWindow;
	if (!this->modelWindow) {
		return false;
	}

	//Initialize the modelWindow object
	result = this->modelWindow->Initialize(this->direct3DH->GetDevice(), this->direct3DH->GetDeviceContext(), screenWidth, screenHeight);
	if (!result) {
		MessageBox(hwnd, L"this->modelWindow->Initialize", L"Error", MB_OK);
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
	
	if (this->moveLight > 5.0f) {
		this->increase = false;
	}
	if (this->moveLight < -30.0f) {
		this->increase = true;
	}
	if (this->increase) {
		this->moveLight += dTime / 80000;
	}
	else {
		this->moveLight -= dTime / 80000;
	}

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
	XMMATRIX worldMatrix, viewMatrix, lightViewMatrix, projectionMatrix, lightProjectionMatrix, orthoMatrix;
	bool result;
	int indexCount;
	int indexStart;
	int modelSubsets;
	int textureIndex;
	int normMapIndex;
	XMFLOAT4 difColor;
	XMFLOAT4 specColor;
	bool transparent;
	XMVECTOR camPosVec = this->cameraH->GetPosition();
	XMFLOAT4 camPos = XMFLOAT4(XMVectorGetX(camPosVec), XMVectorGetY(camPosVec), XMVectorGetZ(camPosVec), XMVectorGetW(camPosVec));
	
	

	//**DEFERRED RENDER**\\

	//Set the render target to be the textures
	this->direct3DH->ChangeRenderTargets(1);

	//Clear the buffers to begin the scene
	this->direct3DH->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	//Get the view, and projection matrices from the camera and d3d objects
	this->cameraH->GetViewMatrix(viewMatrix);
	this->direct3DH->GetProjectionMatrix(projectionMatrix);

	//**NON TRANSPARENT RENDER**\\
	
	//Ground Render
	for (int i = 0; i < this->groundModels.size(); i++) {
		this->groundModels.at(i)->GetWorldMatrix(worldMatrix);

		this->groundModels.at(i)->Render(this->direct3DH->GetDeviceContext());
		modelSubsets = this->groundModels.at(i)->NrOfSubsets();
		for (int j = 0; j < modelSubsets; j++) {
			//Get all the nessecary information from the model
			this->groundModels.at(i)->GetSubsetInfo(j, indexStart, indexCount, textureIndex, normMapIndex, difColor, specColor, transparent);

			//Render the model using the shader-handler
			if (!transparent) {
				result = this->shaderH->Render(this->direct3DH->GetDeviceContext(), indexCount, indexStart,
					worldMatrix, viewMatrix, projectionMatrix, this->groundModels.at(i)->GetTexture(textureIndex),
					this->groundModels.at(i)->GetTexture(normMapIndex), difColor, specColor, false, camPos);
				if (!result)
				{
					return false;
				}

			}
		}
	}

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
					this->models.at(i)->GetTexture(normMapIndex), difColor, specColor, false, camPos);
				if (!result)
				{
					return false;
				}

			}
		}
	}

	//**SHADOW RENDER**\\

	//Set the render target to be the textures
	this->direct3DH->ChangeRenderTargets(2);

	//Change viewport size to shadow map size (2000)
	this->direct3DH->SetShadowViewport(true);

	//Create the view, and projection matrices based on light pos(25, 15, -6)
	XMVECTOR lookAt = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	XMVECTOR lightPos = XMVectorSet(25.0f, 15.0f, this->moveLight, 0.0f);
	XMVECTOR lightUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	lightViewMatrix = XMMatrixLookAtLH(lightPos, lookAt, lightUp);

	float fieldOfView = (float)XM_PI / 2.0f;
	float screenAspect = 1.0f;
	lightProjectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, 1.0f, 100.0f);

	for (int i = 0; i < this->models.size(); i++) {

		//Get the world matrix from model
		this->models.at(i)->GetWorldMatrix(worldMatrix);

		//Put the model1 vertex and index buffers on the graphics pipeline to prepare them for drawing
		this->models.at(i)->Render(this->direct3DH->GetDeviceContext());

		//Draw all non transparent subsets
		modelSubsets = this->models.at(i)->NrOfSubsets();
		for (int j = 0; j < modelSubsets; j++) {
			//Get all the nessecary information from the model
			this->models.at(i)->GetSubsetInfo(j, indexStart, indexCount, textureIndex, normMapIndex, difColor, specColor, transparent);

			//Render the model using the shader-handler
			if (!transparent) {
				result = this->shadowShaderH->Render(this->direct3DH->GetDeviceContext(), indexCount, indexStart,
					worldMatrix, lightViewMatrix, lightProjectionMatrix);
				if (!result)
				{
					return false;
				}

			}
		}
	}

	//Change viewport back
	this->direct3DH->SetShadowViewport(false);

	//**LIGHT RENDER**\\

	//Set the render target to be the back buffer
	this->direct3DH->ChangeRenderTargets(3);

	//Turn off depth buffer for 2d rendering
	this->direct3DH->SetZBuffer(false);

	//Get the model window world matrix
	this->modelWindow->GetWorldMatrix(worldMatrix);

	//Get the base view matrix
	this->cameraH->GetBaseViewMatrix(viewMatrix);

	//Get the ortho matrix for 2d rendering
	this->direct3DH->GetOrthoMatrix(orthoMatrix);

	//Put the model windows buffers on the pipeline
	this->modelWindow->Render(this->direct3DH->GetDeviceContext());

	result = this->colorShaderH->Render(this->direct3DH->GetDeviceContext(), this->modelWindow->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, lightViewMatrix, lightProjectionMatrix,
		this->direct3DH->GetShaderResourceView(0), this->direct3DH->GetShaderResourceView(1), this->direct3DH->GetShaderResourceView(2), this->direct3DH->GetShaderResourceView(3), this->direct3DH->GetShaderResourceView(4),
		camPos, XMFLOAT4(25.0f, 15.0f, this->moveLight, 0.0f));
	if (!result) {
		return false;
	}

	this->direct3DH->SetZBuffer(true);

	////**TRANSPARENT RENDER**\\

	//for (int i = 0; i < this->models.size(); i++) {

	//	//Get the world matrix from model
	//	this->models.at(i)->GetWorldMatrix(worldMatrix);

	//	//Put the model1 vertex and index buffers on the graphics pipeline to prepare them for drawing
	//	this->models.at(i)->Render(this->direct3DH->GetDeviceContext());

	//	//Draw all non transparent subsets
	//	modelSubsets = this->models.at(i)->NrOfSubsets();
	//	for (int j = 0; j < modelSubsets; j++) {

	//		//Get all the nessecary information from the model
	//		models.at(i)->GetSubsetInfo(j, indexStart, indexCount, textureIndex, normMapIndex, difColor, specColor, transparent);

	//		if (transparent) {
	//			//Render the model using the shader-handler
	//			result = this->shaderH->Render(this->direct3DH->GetDeviceContext(), indexCount, indexStart,
	//				worldMatrix, viewMatrix, projectionMatrix, this->models.at(i)->GetTexture(textureIndex),
	//				this->models.at(i)->GetTexture(normMapIndex), difColor, specColor, true, camPos);
	//			if (!result)
	//			{
	//				return false;
	//			}
	//		}
	//	}
	//}


	//Display the rendered scene to screen
	this->direct3DH->EndScene();

	return true;
}

void GraphicsHandler::Shutdown()
{
	//Release the shaderHandler object
	if (this->shaderH) {
		this->shaderH->Shutdown();
		delete this->shaderH;
		this->shaderH = 0;
	}

	//Release the shaderHandler object
	if (this->colorShaderH) {
		this->colorShaderH->Shutdown();
		delete this->colorShaderH;
		this->colorShaderH = 0;
	}

	//Release the Model objects
	for (int i = 0; i < this->models.size(); i++) {
		if (this->models.at(i)) {
			this->models.at(i)->Shutdown();
			delete this->models.at(i);
			this->models.at(i) = nullptr;
		}
	}
	
	//Delete the model for 2d drawing
	if (this->modelWindow) {
		delete this->modelWindow;
		this->modelWindow = nullptr;
	}

	//Release the cameraHandler object
	if (this->cameraH) {
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