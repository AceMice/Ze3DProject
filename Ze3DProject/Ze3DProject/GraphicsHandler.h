#ifndef GRAPHICSHANDLER_H
#define GRAPHICSHANDLER_H

#include "D3DHandler.h"
#include "DeferredShaderHandler.h"
#include "Model.h"
#include "CameraHandler.h"
#include "Inputhandler.h"
#include "GroundModel.h"
#include "ColorShaderHandler.h"
#include "ModelWindow.h"
#include "ShadowShaderHandler.h"
#include "Frustum.h"
#include "ModelHandler.h"
#include "TextHandler.h"

//Constants
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = false;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

class GraphicsHandler {
private:
	D3DHandler* direct3DH;
	CameraHandler* cameraH;
	std::vector<GroundModel*> groundModels;

	DeferredShaderHandler* deferredShaderH;
	ColorShaderHandler* colorShaderH;
	ModelWindow* modelWindow;
	ShadowShaderHandler* shadowShaderH;
	Frustum* frustum;
	ModelHandler* modelHandler;
	TextHandler* textHandler;

	bool Render();
	float moveLight;
	bool increase;
	float runTime;
	int modelsLeft;
	int highscore;

	bool saveHighscore();
	bool loadHighscore();
public:
	GraphicsHandler();
	~GraphicsHandler();

	bool Initialize(int, int, HWND);
	bool Frame(float, InputHandler*, HWND);
	void Shutdown();
	void GetProjectionMatrix(XMMATRIX&);
};

#endif