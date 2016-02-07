#ifndef GRAPHICSHANDLER_H
#define GRAPHICSHANDLER_H

#include "D3DHandler.h"
#include "ShaderHandler.h"

class GraphicsHandler {
private:

public:
	GraphicsHandler();
	~GraphicsHandler();

	bool Initialize();
	void Run();
	void Shutdown();
};

#endif