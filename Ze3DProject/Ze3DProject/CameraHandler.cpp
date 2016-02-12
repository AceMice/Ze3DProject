#include "CameraHandler.h"

CameraHandler::CameraHandler()
{
	this->positionX = 0.0f;
	this->positionY = 0.0f;
	this->positionZ = 0.0f;

	this->rotationX = 0.0f;
	this->rotationY = 0.0f;
	this->rotationZ = 0.0f;
}

CameraHandler::~CameraHandler()
{
}

void CameraHandler::SetPosition(float x, float y, float z)
{
	this->positionX = x;
	this->positionY = y;
	this->positionZ = z;

	return;
}

void CameraHandler::SetRotation(float x, float y, float z)
{
	this->rotationX = x;
	this->rotationY = y;
	this->rotationZ = z;
}

XMFLOAT3 CameraHandler::GetPosition()
{
	return XMFLOAT3(this->positionX, this->positionY, this->positionZ);
}

XMFLOAT3 CameraHandler::GetRotation()
{
	return XMFLOAT3(this->rotationX, this->rotationY, this->rotationZ);
}

void CameraHandler::Render()
{
	XMFLOAT3 up, position, lookAt;
	XMVECTOR upVector, positionVector, lookAtVector;
	float yaw, pitch, roll;
	XMMATRIX rotationMatrix;

	//Setup vector that point up
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	//Load up into a XMVECTOR structure
	upVector = XMLoadFloat3(&up);

	//Setup the position of the camera in the world
	position.x = this->positionX;
	position.y = this->positionY;
	position.z = this->positionZ;

	//Load pos into a XMVECTOR structure
	positionVector = XMLoadFloat3(&position);

	//Setup where the camera is looking default
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;

	//Load lookat into a XMVECTOR structure
	lookAtVector = XMLoadFloat3(&lookAt);

	//Set the yaw (Y axis), pitch (X axis), and roll (Z axis) rotations in radians
	pitch = this->rotationX * 0.0174532925f;
	yaw = this->rotationY * 0.0174532925f;
	roll = this->rotationZ * 0.0174532925f;

	//Create the rotation matrix from the yaw, pitch, and roll values
	rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	//Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin.
	lookAtVector = XMVector3TransformCoord(lookAtVector, rotationMatrix);
	upVector = XMVector3TransformCoord(upVector, rotationMatrix);

	//Translate the rotated camera position to the location of the viewer
	lookAtVector = XMVectorAdd(positionVector, lookAtVector);

	//Finally create the view matrix from the three updated vectors
	this->viewMatrix = XMMatrixLookAtLH(positionVector, lookAtVector, upVector);

	return;
}

void CameraHandler::GetViewMatrix(XMMATRIX& viewMatrix)
{
	viewMatrix = this->viewMatrix;

	return;
}