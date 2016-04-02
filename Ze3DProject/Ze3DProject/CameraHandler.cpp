#include "CameraHandler.h"

CameraHandler::CameraHandler()
{
	this->camPos = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

	this->baseViewMatrix = XMMatrixIdentity();
	this->viewMatrix = XMMatrixIdentity();
	this->camRotationMatrix = XMMatrixIdentity();
	
	this->moveLeftRight = 0.0f;
	this->moveBackForward = 0.0f;
	this->moveUpDown = 0.0f;
	this->camYaw = 0.0f;
	this->camPitch = 0.0f;
	this->camRoll = 0.0f;
	this->lockToTerrain = true;
}

CameraHandler::~CameraHandler()
{
}

void CameraHandler::SetPosition(float x, float y, float z)
{
	this->camPos = XMVectorSet(x, y, z, 0);

	return;
}

XMVECTOR CameraHandler::GetPosition()
{
	return this->camPos;
}

void CameraHandler::updateCamera(float dt, InputHandler* inputH, GroundModel*model) {

	float speed = 90000;

	if (inputH->IsKeyDown(87)) {	//W
		this->moveBackForward += dt/speed;
	}

	if (inputH->IsKeyDown(83)) {	//S
		this->moveBackForward -= dt/speed;
	}

	if (inputH->IsKeyDown(65)) {	//A
		this->moveLeftRight -= dt/speed;
	}

	if (inputH->IsKeyDown(68)) {	//D
		this->moveLeftRight += dt/speed;
	}

	if (inputH->IsKeyDown(69)) {	//E
		this->camYaw += 0.02;
	}

	if (inputH->IsKeyDown(81)) {	//Q
		this->camYaw -= 0.02;
	}

	if (inputH->IsKeyReleased(VK_SPACE)) {	//SPACE
		if (this->lockToTerrain == true) {
			this->lockToTerrain = false;
		}
		else {
			this->lockToTerrain = true;
		}
	}

	if (inputH->IsKeyDown(49)) {	//1
		this->moveUpDown += dt / speed;
	}

	if (inputH->IsKeyDown(50)) {	//2
		this->moveUpDown -= dt/speed;
	}
	
	if (this->lockToTerrain == true) {
		this->CameraMeshIntersect(model);
	}
	
	//Change Pitch/yaw values depending on mouse movement
	this->camPitch += (XMVectorGetY(inputH->GetMouseDeltaPos()) * 0.005);
	if (this->camPitch > 1.5f) {
		this->camPitch = 1.5f;
	}
	else if (this->camPitch < -1.5f) {
		this->camPitch = -1.5f;
	}
	this->camYaw += (XMVectorGetX(inputH->GetMouseDeltaPos()) * 0.005);

	return;
}

void CameraHandler::Frame(float dt, InputHandler* inputH, GroundModel* model)
{
	XMVECTOR lookAt = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	XMVECTOR camRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR camUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	//Get input from keyboard and mouse movement to change camera values
	this->updateCamera(dt, inputH, model);

	this->camRotationMatrix = XMMatrixRotationRollPitchYaw(this->camPitch, this->camYaw, 0);

	lookAt = XMVector3TransformCoord(lookAt, this->camRotationMatrix);

	lookAt = XMVector3Normalize(lookAt);

	camRight = XMVector3TransformCoord(camRight, this->camRotationMatrix);
	camUp = XMVector3TransformCoord(camUp, this->camRotationMatrix);

	
	//Set new camPos
	this->camPos += this->moveLeftRight * camRight;
	this->camPos += this->moveBackForward * lookAt;
	this->camPos += this->moveUpDown * camUp;

	//Reset
	this->moveLeftRight = 0.0f;
	this->moveBackForward = 0.0f;
	this->moveUpDown = 0.0f;

	lookAt = this->camPos + lookAt;

	this->viewMatrix = XMMatrixLookAtLH(this->camPos, lookAt, camUp);

	return;
}

void CameraHandler::GetViewMatrix(XMMATRIX& viewMatrix)
{
	viewMatrix = this->viewMatrix;

	return;
}

void CameraHandler::GenerateBaseViewMatrix()
{
	XMVECTOR lookAt = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	XMVECTOR camRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR camUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	this->camRotationMatrix = XMMatrixRotationRollPitchYaw(this->camPitch, this->camYaw, 0);

	lookAt = XMVector3TransformCoord(lookAt, this->camRotationMatrix);

	lookAt = XMVector3Normalize(lookAt);

	camRight = XMVector3TransformCoord(camRight, this->camRotationMatrix);
	camUp = XMVector3TransformCoord(camUp, this->camRotationMatrix);
	

	//Set new camPos
	this->camPos += this->moveLeftRight * camRight;
	this->camPos += this->moveBackForward * lookAt;
	this->camPos += this->moveUpDown * camUp;

	//Reset
	this->moveLeftRight = 0.0f;
	this->moveBackForward = 0.0f;
	this->moveUpDown = 0.0f;

	lookAt = this->camPos + lookAt;

	this->baseViewMatrix = XMMatrixLookAtLH(this->camPos, lookAt, camUp);

	return;
}

void CameraHandler::GetBaseViewMatrix(XMMATRIX& baseViewMatrix)
{
	baseViewMatrix = this->baseViewMatrix;

	return;
}

bool CameraHandler::CameraMeshIntersect(GroundModel* model) {
	int fp;
	float y1;
	float y2;
	float y3;
	float avY;
	bool result = false;
	XMVECTOR tempCamPos = this->GetPosition();
	XMFLOAT3 camLocalPos;
	XMMATRIX modelWorldMatrix;
	XMMATRIX modelWorldMatrixInverse;
	int modelWidth = model->getHeightMapInfo().terrainWidth;
	int modelHeight = model->getHeightMapInfo().terrainHeight;

	//Move camera into the models local space
	model->GetWorldMatrix(modelWorldMatrix);
	
	HeightMap::HeightMapInfo hmInfo = model->getHeightMapInfo();

	modelWorldMatrixInverse = XMMatrixInverse(nullptr,modelWorldMatrix);
	
	XMVECTOR camPosLocalMesh = XMVector3TransformCoord(tempCamPos, modelWorldMatrixInverse);	//CamPos in the mesh local space
	
	XMStoreFloat3(&camLocalPos, camPosLocalMesh);
	
	//Check if we are inside the x and z
	if ((camLocalPos.x >= 0 && camLocalPos.x <= modelWidth) &&
		(camLocalPos.z >= 0 && camLocalPos.z <= modelHeight)
		) 
	{

		//Find whitch triangel we are inside
		if ((camLocalPos.x - (int)camLocalPos.x) + (camLocalPos.z - (int)camLocalPos.z) <= 1.0f) {	// Top left triangle
			
			fp = ((int)camLocalPos.x) + ((int) camLocalPos.z * modelWidth);
			y1 = hmInfo.heightMap[fp].y;
			
			if ((int)camLocalPos.z == modelHeight-1) {
				y2 = 0;
			}
			else {
				y2 = hmInfo.heightMap[fp + modelWidth].y;
			}

			if ((int)camLocalPos.x == modelWidth) {
				y3 = 0;
			}
			else {
				y3 = hmInfo.heightMap[fp + 1].y;
			}

			avY = (y1 + y2 + y3) / 3;
			if (avY > 1000) {
				int j = 0;
			}
			this->SetPosition(XMVectorGetX(this->GetPosition()), avY, XMVectorGetZ(this->GetPosition()));
		}
		else {										// Bottom right triangle
			int fp = ((int)camLocalPos.x) + ((int)camLocalPos.z * modelWidth);
			y1 = hmInfo.heightMap[fp].y;
			
			if ((int)camLocalPos.z == 0) {
				y2 = 0;
			}
			else {
				y2 = hmInfo.heightMap[fp - modelHeight].y;
			}

			if ((int)camLocalPos.x == 0) {
				y3 = 0;
			}
			else {
				y3 = hmInfo.heightMap[fp - 1].y;
			}
			
			avY = (y1 + y2 + y3) / 3;
			if (avY > 1000) {
				int j = 0;
			}

		}

		XMVECTOR tempV = XMVectorSet(1,avY, 1, 1);
		XMVector3TransformCoord(tempV, modelWorldMatrix);

		this->SetPosition(XMVectorGetX(this->GetPosition()), XMVectorGetY(tempV), XMVectorGetZ(this->GetPosition()));
		result = true;
	}
	
	//Check if the point is inside the triangel plane 

	// return bool if it is inside or not which will cancel the questioned action in updateCamera
	return result;
}
