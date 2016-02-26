#ifndef CAMERAHANDLER_H
#define CAMERAHANDLER_H

#include <DirectXMath.h>
#include "InputHandler.h"
using namespace DirectX;

class CameraHandler
{
private:

	XMVECTOR camPos;

	XMMATRIX viewMatrix;
	XMMATRIX camRotationMatrix;
	
	float moveLeftRight;
	float moveBackForward;
	float moveUpDown;
	float camYaw;
	float camPitch;
	float camRoll;
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