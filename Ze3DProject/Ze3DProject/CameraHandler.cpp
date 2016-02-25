#include "CameraHandler.h"

CameraHandler::CameraHandler()
{
	this->camPos = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	this->up = XMVectorSet(0.0f,1.0f,0.0f,0.0f);

	this->DefaultForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	this->DefaultRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	this->camForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	this->camRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

	this->lookAt = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	this->moveLeftRight = 0.0f;
	this->moveBackForward = 0.0f;
	this->moveUpDown = 0.0f;
	this->camYaw = 0.0f;
	this->camPitch = 0.0f;

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
		this->camYaw += 0.01;
	}

	if (inputH->IsKeyDown(81)) {	//Q
		this->camYaw -= 0.01;
	}

	if (inputH->IsKeyDown(49)) {	//1
		this->moveUpDown += dt/speed;
	}

	if (inputH->IsKeyDown(50)) {	//2
		this->moveUpDown -= dt/speed;
	}

	return;
}

void CameraHandler::Frame(float dt, InputHandler* inputH)
{
	this->updateCamera(dt, inputH);

	this->camRotationMatrix = XMMatrixRotationRollPitchYaw(this->camPitch, this->camYaw, 0);
	this->lookAt = XMVector3TransformCoord(this->DefaultForward, this->camRotationMatrix);

	this->lookAt = XMVector3Normalize(this->lookAt);

	this->camRight = XMVector3TransformCoord(DefaultRight, this->camRotationMatrix);
	this->up = XMVector3TransformCoord(this->up, this->camRotationMatrix);
	this->camForward = XMVector3TransformCoord(this->DefaultForward, this->camRotationMatrix);
	
	//Set new camPos
	this->camPos += this->moveLeftRight * this->camRight;
	this->camPos += this->moveBackForward * this->camForward;
	this->camPos += this->moveUpDown * this->up;

	//Reset
	this->moveLeftRight = 0.0f;
	this->moveBackForward = 0.0f;
	this->moveUpDown = 0.0f;

	this->lookAt = this->camPos + this->lookAt;

	this->viewMatrix = XMMatrixLookAtLH(this->camPos, this->lookAt, this->up);

	return;
}

void CameraHandler::GetViewMatrix(XMMATRIX& viewMatrix)
{
	viewMatrix = this->viewMatrix;

	return;
}