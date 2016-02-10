#include "ShaderHandler.h"

ShaderHandler::ShaderHandler()
{
	this->vertexShader = nullptr;
	this->pixelShader = nullptr;
	this->layout = nullptr;
	this->matrixBuffer = nullptr;
}

ShaderHandler::~ShaderHandler()
{
}

bool ShaderHandler::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result = false;

	result = this->InitializeShader(device, hwnd, L"../Ze3DProject/VertexShader.hlsl", L"../Ze3DProject/PixelShader.hlsl");
	if (!result) {
		return false;
	}

	return true;
}

void ShaderHandler::Shutdown()
{
	this->ShutdownShader();

	return;
}