#ifndef GRAPHICSHANDLER_H
#define GRAPHICSHANDLER_H

#include "D3DHandler.h"
#include "ShaderHandler.h"

//Constants
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = false;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

class GraphicsHandler {
private:


	bool Render();
public:
	GraphicsHandler();
	~GraphicsHandler();

	bool Initialize();
	void Run();
	void Shutdown();
};

#endif