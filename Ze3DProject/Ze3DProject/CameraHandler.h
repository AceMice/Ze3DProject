#ifndef CAMERAHANDLER_H
#define CAMERAHANDLER_H

#include <DirectXMath.h>
#include "InputHandler.h"
using namespace DirectX;

class CameraHandler
{
private:

	XMVECTOR lookAt;
	XMVECTOR up;
	XMVECTOR camPos;
	XMMATRIX viewMatrix;

	XMMATRIX camRotationMatrix;
	XMVECTOR DefaultForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	XMVECTOR DefaultRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR camForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	XMVECTOR camRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	float moveLeftRight;
	float moveBackForward;
	float moveUpDown;
	float camYaw;
	float camPitch;

public:
	CameraHandler();
	~CameraHandler();

	void SetPosition(float, float, float);
	XMVECTOR GetPosition();

	void Frame(float, InputHandler*);
	void updateCamera(float, InputHandler*);
	void GetViewMatrix(XMMATRIX&);
};

#endif