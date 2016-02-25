#ifndef CAMERAHANDLER_H
#define CAMERAHANDLER_H

#include <DirectXMath.h>
#include "InputHandler.h"
using namespace DirectX;

class CameraHandler
{
private:
	float positionX, positionY, positionZ;
	float rotationX, rotationY, rotationZ;
	XMMATRIX viewMatrix;

public:
	CameraHandler();
	~CameraHandler();

	void SetPosition(float, float, float);
	void SetRotation(float, float, float);

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetRotation();

	void Render(InputHandler*);
	void updateCamera(InputHandler*);
	void GetViewMatrix(XMMATRIX&);
};

#endif