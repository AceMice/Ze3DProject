#include "GraphicsHandler.h"

GraphicsHandler::GraphicsHandler()
{
	this->direct3DH = nullptr;
	this->cameraH = nullptr;
	this->deferredShaderH = nullptr;
	this->colorShaderH = nullptr;
	this->modelWindow = nullptr;
	this->shadowShaderH = nullptr;
	this->frustum = nullptr;
	this->modelHandler = nullptr;
	this->textHandler = nullptr;

	this->moveLight = 0.0f;
	this->increase = true;
	this->runTime = 0.0f;
	this->modelsLeft = 0;
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
	this->cameraH->SetPosition(0.0f, 0.0f, -20.0f, true);
	this->cameraH->GenerateBaseViewMatrix();

	//Create the GroundModel Object
	tempGroundModel = new GroundModel;

	result = tempGroundModel->Initialize(this->direct3DH->GetDevice(), this->direct3DH->GetDeviceContext(), "Vally24Bit");
	if (!result)
	{
		MessageBox(hwnd, L"this->groundModel->Initialize", L"Error", MB_OK);
		return false;
	}
	this->groundModel = tempGroundModel;
	tempGroundModel = nullptr;

	//Create the modelhandler object
	this->modelHandler = new ModelHandler;
	if (!this->modelHandler) {
		return false;
	}

	std::stringstream ss;
	for (int i = 0; i < 10; i++) {
		//Create the box objects
		ss.str("");
		ss << i;
		std::string boxName = "WoodBox" + ss.str();
		ss.clear();
		result = this->modelHandler->CreateModel(this->direct3DH->GetDevice(), this->direct3DH->GetDeviceContext(), "WoodBox", boxName, true);
		if (!result)
		{
			MessageBox(hwnd, L"this->modelHandler->CreateModelWoodBox", L"Error", MB_OK);
			return false;
		}

	}

	//Create the ogre
	result = this->modelHandler->CreateModel(this->direct3DH->GetDevice(), this->direct3DH->GetDeviceContext(), "ogreFullG", "ogreFullG", false);
	if (!result)
	{
		MessageBox(hwnd, L"this->modelHandler->CreateModelogreFullG", L"Error", MB_OK);
		return false;
	}


	//Create the car
	result = this->modelHandler->CreateModel(this->direct3DH->GetDevice(), this->direct3DH->GetDeviceContext(), "carSLS3", "carSLS3", false);
	if (!result)
	{
		MessageBox(hwnd, L"this->modelHandler->CreateModelcarSLS3", L"Error", MB_OK);
		return false;
	}

	// Create the deferred shader object.
	this->deferredShaderH = new DeferredShaderHandler;
	if (!this->deferredShaderH) {
		return false;
	}

	// Initialize the deferred shader object.
	result = this->deferredShaderH->Initialize(this->direct3DH->GetDevice(), hwnd);
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

	this->frustum = new Frustum;
	if (!this->frustum) {
		return false;
	}

	XMMATRIX modelWorld;
	
	float z = 0.0f;
	float x = 0.0f;
	for (int i = 0; i < 10; i++) { //Move the 10 boxes created earlier
		std::string boxName;
		ss.str("");
		ss << i;
		boxName = "WoodBox" + ss.str();
		z = (i / 5 * 40) + 50;
		x = ((i % 5) * 40) + 20;
		modelWorld = XMMatrixScaling(0.25f, 0.25f, 0.25f);
		modelWorld = XMMatrixTranslation(x, -7.15f, z) * modelWorld;
		if (!this->modelHandler->UpdateModelWorldMatrix(boxName, modelWorld)) {
			return false;
		}

		ss.clear();
	}
	

	modelWorld = XMMatrixScaling(0.7f, 0.7f, 0.7f);
	modelWorld = XMMatrixTranslation(0, -5.75f, 10) * modelWorld;
	modelWorld = XMMatrixRotationY(1.6f) * modelWorld;
	if (!this->modelHandler->UpdateModelWorldMatrix("ogreFullG", modelWorld)) {
		return false;
	}

	modelWorld = XMMatrixScaling(4.25f, 4.25f, 4.25f);
	modelWorld = XMMatrixTranslation(-1.5f, -0.5f, 2.5f) * modelWorld;
	if (!this->modelHandler->UpdateModelWorldMatrix("carSLS3", modelWorld)) {
		return false;
	}

	modelWorld = XMMatrixTranslation(-30.0f, -4.0f, -40.0f);
	this->groundModel->SetWorldMatrix(modelWorld);

	XMMATRIX viewMatrix, projectionMatrix;
	this->cameraH->GetBaseViewMatrix(viewMatrix);

	//Generate the models min max vertices and create the quadtree
	this->modelHandler->GenerateModelsMinMaxVerts();
	this->modelHandler->CreateQuadTree(this->direct3DH->GetDevice(), this->direct3DH->GetDeviceContext(), 1);

	this->textHandler = new TextHandler;
	if (!this->textHandler) {
		return false;
	}
	result = this->textHandler->Initialize(this->direct3DH->GetDevice(), this->direct3DH->GetDeviceContext(), viewMatrix, screenWidth, screenHeight);
	if (!result) {
		return false;
	}
	int sentenceId = this->textHandler->CreateSentence(this->direct3DH->GetDevice(), 32);
	if (sentenceId == -1) {
		return false;
	}
	result = this->textHandler->UpdateSentence(this->direct3DH->GetDeviceContext(), 0, "Hello World!", 100, 100, XMFLOAT3(1.0f, 0.0f, 0.0f));
	if (!result) {
		return false;
	}
	sentenceId = this->textHandler->CreateSentence(this->direct3DH->GetDevice(), 20);
	if (sentenceId == -1) {
		return false;
	}
	sentenceId = this->textHandler->CreateSentence(this->direct3DH->GetDevice(), 32);
	if (sentenceId == -1) {
		return false;
	}
	sentenceId = this->textHandler->CreateSentence(this->direct3DH->GetDevice(), 16);
	if (sentenceId == -1) {
		return false;
	}

	if (!this->loadHighscore()) {
		this->highscore = 1337;
	}


	return true;
}

bool GraphicsHandler::Frame(float dTime, InputHandler* inputH, HWND hwnd)
{
	bool result;
	XMMATRIX modelWorld;
	Model* tempModel = nullptr;
	XMMATRIX projM;
	this->direct3DH->GetProjectionMatrix(projM);
	
	if (this->moveLight > 35.0f) {
		this->increase = false;
	}
	if (this->moveLight < 0.0f) {
		this->increase = true;
	}
	//this->moveLight = -10.0f;
	if (this->increase) {
		this->moveLight += dTime / 80000;
	}
	else {
		this->moveLight -= dTime / 80000;
	}

	if (this->runTime == -1) {
		this->runTime = 0;
	}
	else {
		this->runTime += dTime / 1000000;
	}

	//Generate the view matrix based on the camera's position
	this->cameraH->Frame(dTime, inputH, this->groundModel);	//Sending down the mesh to check if it and the camera intersect
	
	if (this->modelHandler->GetNrPickableModels() == 0) {
		int playAgain;
		std::wstring text;
		if (this->runTime < this->highscore) {
			this->highscore = (int)this->runTime;
			this->saveHighscore();
			text = L"You beat the game highscore with a time of " + std::to_wstring(this->highscore) + L", well painted!\nDo you want to play again?";
			playAgain = MessageBox(hwnd, text.c_str(), L"Congratulations!", MB_YESNO);
		}
		else {
			text = L"Good job, you painted the boxes in " + std::to_wstring((int)this->runTime) + L" seconds so not quite enough for a highscore!\nDo you want to play again?";
			playAgain = MessageBox(hwnd, text.c_str(), L"Well done!", MB_YESNO);
		}
		
		if (playAgain == 7) {
			return false;
		}
		else {
			this->runTime = -1;
			this->modelHandler->resetSelectedModels();
			this->cameraH->SetPosition(0.0f, 0.0f, -20.0f, true);
		}
	}

	//Picking
	if (inputH->IsKeyReleased(69)) {	//E
	
		this->modelHandler->SelectModel(inputH->GetMouseViewPos(projM), this->cameraH);
	}

	std::string text = "Time: " + std::to_string((int)this->runTime);

	result = this->textHandler->UpdateSentence(this->direct3DH->GetDeviceContext(), 0, text, 50, 50, XMFLOAT3(1.0f, 0.0f, 0.0f));
	if (!result) {
		return false;
	}

	this->modelsLeft = this->modelHandler->GetNrPickableModels();

	text = "Left to paint: " + std::to_string(this->modelsLeft);

	result = this->textHandler->UpdateSentence(this->direct3DH->GetDeviceContext(), 1, text, 50, 70, XMFLOAT3(1.0f, 0.0f, 0.0f));
	if (!result) {
		return false;

	}

	text = "Hightscore: " + std::to_string(this->highscore);

	result = this->textHandler->UpdateSentence(this->direct3DH->GetDeviceContext(), 2, text, 50, 90, XMFLOAT3(1.0f, 0.0f, 0.0f));
	if (!result) {
		return false;
	}

	text = "FPS: " + std::to_string((int)(1000000/dTime));

	result = this->textHandler->UpdateSentence(this->direct3DH->GetDeviceContext(), 3, text, 50, 30, XMFLOAT3(1.0f, 0.0f, 0.0f));
	if (!result) {
		return false;
	}

	result = this->Render();
	if (!result) {
		return false;
	}

	return true;
}

bool GraphicsHandler::Render()
{
	XMMATRIX worldMatrix, viewMatrix, lightViewMatrix, projectionMatrix, lightProjectionMatrix, orthoMatrix, MVP;
	bool result;
	int indexCount;
	int indexStart;
	int modelSubsets;
	int textureIndex;
	int normMapIndex;
	XMFLOAT4 difColor;
	XMFLOAT4 specColor;
	bool transparent;
	bool picked;
	XMVECTOR* modelBB;
	bool renderModel = true;
	int modelsRendered = 0;
	std::vector<Model*> models;

	XMVECTOR camPosVec = this->cameraH->GetPosition();
	XMFLOAT4 camPos = XMFLOAT4(XMVectorGetX(camPosVec), XMVectorGetY(camPosVec), XMVectorGetZ(camPosVec), XMVectorGetW(camPosVec));
	
	//Create the view, and projection matrices based on light pos(25, 15, -6)
	XMVECTOR lookAt = XMVectorSet(0.0f, 0.0f, 15.0f, 0.0f);
	XMVECTOR lightPos = XMVectorSet(-40.0f, 50.0f, this->moveLight, 0.0f);
	XMVECTOR lightUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	lightViewMatrix = XMMatrixLookAtLH(lightPos, lookAt, lightUp);

	float fieldOfView = (float)XM_PI / 4.0f;
	float screenAspect = 1.0f;
	lightProjectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, 1.0f, 1000.0f);

	//**DEFERRED RENDER**\\

	//Clear the buffers to begin the scene
	this->direct3DH->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	//Set the render target to be the textures
	this->direct3DH->ChangeRenderTargets(1);

	//Get the view, and projection matrices from the camera and d3d objects
	this->cameraH->GetViewMatrix(viewMatrix);
	this->direct3DH->GetProjectionMatrix(projectionMatrix);
	
	//Ground Render
	
	this->groundModel->GetWorldMatrix(worldMatrix);

	this->groundModel->Render(this->direct3DH->GetDeviceContext());
	modelSubsets = this->groundModel->NrOfSubsets();
	for (int j = 0; j < modelSubsets; j++) {
		//Get all the nessecary information from the model
		this->groundModel->GetSubsetInfo(j, indexStart, indexCount, textureIndex, normMapIndex, difColor, specColor, transparent);

		//Render the model using the shader-handler
		if (!transparent) {
			result = this->deferredShaderH->Render(this->direct3DH->GetDeviceContext(), indexCount, indexStart,
				worldMatrix, viewMatrix, projectionMatrix, this->groundModel->GetTexture(textureIndex),
				this->groundModel->GetTexture(normMapIndex), difColor, specColor, false, false, camPos);
			if (!result)
			{
				return false;
			}
		}
	}

	//Create the frustum
	this->frustum->CreateFrustum(SCREEN_DEPTH, viewMatrix, projectionMatrix);

	//Get the models in node info
	int path[2] = { 4, 0 };
	int levels = 1;
	
	models = this->modelHandler->GetModels();
	//models = this->modelHandler->GetModelsInViewFrustum(this->frustum);
	//models = this->modelHandler->GetModelsInNode(path, levels);

	for (int i = 0; i < models.size(); i++) {
		models.at(i)->GetWorldMatrix(worldMatrix);
			
		//Put the model1 vertex and index buffers on the graphics pipeline to prepare them for drawing
		models.at(i)->Render(this->direct3DH->GetDeviceContext());

		//Draw all non transparent subsets
		modelSubsets = models.at(i)->NrOfSubsets();
		for (int j = 0; j < modelSubsets; j++) {
			//Get all the nessecary information from the model
			models.at(i)->GetSubsetInfo(j, indexStart, indexCount, textureIndex, normMapIndex, difColor, specColor, transparent, picked);

			//Render the model using the shader-handler
			if (!transparent) {
				result = this->deferredShaderH->Render(this->direct3DH->GetDeviceContext(), indexCount, indexStart,
					worldMatrix, viewMatrix, projectionMatrix, models.at(i)->GetTexture(textureIndex),
					models.at(i)->GetTexture(normMapIndex), difColor, specColor, false, picked, camPos);
				if (!result)
				{
					return false;
				}
			}
		}
		modelsRendered++;
	}


	//**SHADOW RENDER**\\

	//Set the render target to be the textures
	this->direct3DH->ChangeRenderTargets(2);

	//Change viewport size to shadow map size (2000)
	this->direct3DH->SetShadowViewport(true);

	//Ground Render

	this->groundModel->GetWorldMatrix(worldMatrix);

	this->groundModel->Render(this->direct3DH->GetDeviceContext());
	modelSubsets = this->groundModel->NrOfSubsets();
	for (int j = 0; j < modelSubsets; j++) {
		//Get all the nessecary information from the model
		this->groundModel->GetSubsetInfo(j, indexStart, indexCount, textureIndex, normMapIndex, difColor, specColor, transparent);

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
	

	for (int i = 0; i < models.size(); i++) {

		//Get the world matrix from model
		models.at(i)->GetWorldMatrix(worldMatrix);

		//Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing
		models.at(i)->Render(this->direct3DH->GetDeviceContext());

		modelSubsets = models.at(i)->NrOfSubsets();
		for (int j = 0; j < modelSubsets; j++) {
			//Get all the nessecary information from the model
			models.at(i)->GetSubsetInfo(j, indexStart, indexCount, textureIndex, normMapIndex, difColor, specColor, transparent, picked);

			//Render the model using the shadow shaderhandler
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

	XMFLOAT4 lightPosFloat = XMFLOAT4(XMVectorGetX(lightPos), XMVectorGetY(lightPos), XMVectorGetZ(lightPos), XMVectorGetW(lightPos));

	result = this->colorShaderH->Render(this->direct3DH->GetDeviceContext(), this->modelWindow->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, lightViewMatrix, lightProjectionMatrix,
		this->direct3DH->GetShaderResourceView(0), this->direct3DH->GetShaderResourceView(1), this->direct3DH->GetShaderResourceView(2), this->direct3DH->GetShaderResourceView(3), this->direct3DH->GetShaderResourceView(4),
		camPos, lightPosFloat);
	if (!result) {
		return false;
	}

	this->direct3DH->SetZBuffer(true);


	this->textHandler->Render(this->direct3DH->GetDeviceContext(), orthoMatrix);


	//Display the rendered scene to screen
	this->direct3DH->EndScene();

	return true;
}

void GraphicsHandler::Shutdown()
{
	//Release the shaderHandler object
	if (this->deferredShaderH) {
		this->deferredShaderH->Shutdown();
		delete this->deferredShaderH;
		this->deferredShaderH = nullptr;
	}

	//Release the shaderHandler object
	if (this->colorShaderH) {
		this->colorShaderH->Shutdown();
		delete this->colorShaderH;
		this->colorShaderH = nullptr;
	}
	//Release the shaderHandler object
	if (this->shadowShaderH) {
		this->shadowShaderH->Shutdown();
		delete this->shadowShaderH;
		this->shadowShaderH = nullptr;
	}

	//Release the Model objects

	if (this->groundModel) {
		this->groundModel->Shutdown();
		delete this->groundModel;
		this->groundModel = nullptr;
	}

	if (this->modelHandler) {
		this->modelHandler->Shutdown();
		delete this->modelHandler;
		this->modelHandler = nullptr;
	}

	if (this->frustum) {
		delete this->frustum;
		this->frustum = nullptr;
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

	if (!this->textHandler) {
		this->textHandler->Shutdown();
		delete this->textHandler;
		this->textHandler = nullptr;
	}

	//Release the direct
	if (this->direct3DH) {
		this->direct3DH->Shutdown();
		delete this->direct3DH;
		this->direct3DH = nullptr;
	}
}

void GraphicsHandler::GetProjectionMatrix(XMMATRIX &projectionMatrix) {
	this->direct3DH->GetProjectionMatrix(projectionMatrix);
}

bool GraphicsHandler::loadHighscore()
{
	std::ifstream file;
	std::string line;
	std::stringstream ss;
	file.open("../Ze3DProject/Data/highscore.txt");
	if (!file.is_open()) {
		return false;
	}

	std::getline(file, line);
	ss.str(line);
	ss >> this->highscore;
	ss.clear();

	file.close();

	return true;
}

bool GraphicsHandler::saveHighscore()
{
	std::ofstream file;
	std::string line;
	std::stringstream ss;
	file.open("../Ze3DProject/Data/highscore.txt");
	if (!file.is_open()) {
		return false;
	}

	file << this->highscore;

	file.close();
	
	return true;
}