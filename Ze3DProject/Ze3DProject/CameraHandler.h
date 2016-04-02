#ifndef CAMERAHANDLER_H
#define CAMERAHANDLER_H

#include <DirectXMath.h>
#include "InputHandler.h"
#include "GroundModel.h"

using namespace DirectX;

class CameraHandler
{
private:

	XMVECTOR camPos;

	XMMATRIX viewMatrix;
	XMMATRIX baseViewMatrix;
	XMMATRIX camRotationMatrix;
	
	float moveLeftRight;
	float moveBackForward;
	float moveUpDown;
	float camYaw;
	float camPitch;
	float camRoll;
	bool lockToTerrain;

public:
	CameraHandler();
	~CameraHandler();

	void SetPosition(float, float, float, bool resetRotation);
	XMVECTOR GetPosition();

	void Frame(float, InputHandler*, GroundModel*);
	void updateCamera(float, InputHandler*, GroundModel*);
	void GetViewMatrix(XMMATRIX&);
	void GenerateBaseViewMatrix();
	void GetBaseViewMatrix(XMMATRIX&);
	bool CameraMeshIntersect(GroundModel*);

};

#endif