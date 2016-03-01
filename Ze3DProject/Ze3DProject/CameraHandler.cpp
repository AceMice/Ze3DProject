#include "CameraHandler.h"

CameraHandler::CameraHandler()
{
	this->camPos = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

	this->viewMatrix = XMMatrixIdentity();
	this->camRotationMatrix = XMMatrixIdentity();
	
	this->moveLeftRight = 0.0f;
	this->moveBackForward = 0.0f;
	this->moveUpDown = 0.0f;
	this->camYaw = 0.0f;
	this->camPitch = 0.0f;
	this->camRoll = 0.0f;
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

void CameraHandler::updateCamera(float dt, InputHandler* inputH) {

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

	if (inputH->IsKeyDown(49)) {	//1
		this->moveUpDown += dt/speed;
	}

	if (inputH->IsKeyDown(50)) {	//2
		this->moveUpDown -= dt/speed;
	}

	this->camPitch = this->camPitch + (XMVectorGetY(inputH->GetMouseDeltaPos()) * 0.005);
	this->camYaw = this->camYaw + (XMVectorGetX(inputH->GetMouseDeltaPos()) * 0.005);

	return;
}

void CameraHandler::Frame(float dt, InputHandler* inputH)
{
	XMVECTOR lookAt = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	XMVECTOR camRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR camUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	this->updateCamera(dt, inputH);

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