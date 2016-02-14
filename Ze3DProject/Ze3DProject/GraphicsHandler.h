#ifndef GRAPHICSHANDLER_H
#define GRAPHICSHANDLER_H

#include "D3DHandler.h"
#include "ShaderHandler.h"
#include "Model.h"
#include "CameraHandler.h"

//Constants
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = false;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

class GraphicsHandler {
private:
	D3DHandler* direct3DH;
	CameraHandler* cameraH;
	Model* modelH;
	ShaderHandler* shaderH;

	bool Render();
public:
	GraphicsHandler();
	~GraphicsHandler();

	bool Initialize(int, int, HWND);
	bool Frame();
	void Shutdown();
};

#endif