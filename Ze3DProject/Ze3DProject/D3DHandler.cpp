#include "D3DHandler.h"

D3DHandler::D3DHandler()
{
	this->device = nullptr;
	this->deviceContext = nullptr;
	this->swapChain = nullptr;
	this->renderTargetView = nullptr;
	this->depthStencilBuffer = nullptr;
	this->depthStencilState = nullptr;
	this->depthStencilView = nullptr;
	this->rasterState = nullptr;
}

D3DHandler::~D3DHandler()
{
}

