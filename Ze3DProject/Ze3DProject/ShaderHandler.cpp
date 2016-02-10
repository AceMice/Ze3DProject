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

	//Initialize vertex and pixel shaders
	result = this->InitializeShader(device, hwnd, L"../Ze3DProject/VertexShader.hlsl", L"../Ze3DProject/PixelShader.hlsl");
	if (!result) {
		return false;
	}

	return true;
}

void ShaderHandler::Shutdown()
{
	//Shutdown vertex and pixel shaders
	this->ShutdownShader();

	return;
}

bool ShaderHandler::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix)
{
	bool result = false;

	//Set shader parameters used for rendering
	result = this->SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix);
	if (!result) {
		return false;
	}

	this->RenderShader(deviceContext, indexCount);

	return true;
}

bool ShaderHandler::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	return true;
}

bool ShaderHandler::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
	XMMATRIX projectionMatrix)
{
	return true;
}

void ShaderHandler::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	return;
}

void ShaderHandler::ShutdownShader()
{
	//Release matrix constant buffer
	if (this->matrixBuffer) {
		this->matrixBuffer->Release();
		this->matrixBuffer = nullptr;
	}
	//Release layout
	if (this->layout) {
		this->layout->Release();
		this->layout = nullptr;
	}
	//Release pixel shader
	if (this->pixelShader) {
		this->pixelShader->Release();
		this->pixelShader = nullptr;
	}
	//Release vertex shader
	if (this->vertexShader) {
		this->vertexShader->Release();
		this->vertexShader = nullptr;
	}

	return;
}