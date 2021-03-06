#ifndef DEFERREDSHADERHANDLER_H
#define DEFERREDSHADERHANDLER_H

#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <fstream>

using namespace DirectX;
using namespace std;

class DeferredShaderHandler {
private:
	struct MatrixBuffer
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;

		XMFLOAT4 difColor;
		XMFLOAT4 specColor;

		BOOL hasTexture;
		BOOL hasNormMap;
		BOOL picked;

		float padding[1];

		XMFLOAT4 camPos;
	};

	ID3D11VertexShader* vertexShader;
	ID3D11GeometryShader* geoShader;
	ID3D11PixelShader* pixelShader;
	ID3D11InputLayout* layout;
	ID3D11Buffer* matrixBuffer;
	ID3D11SamplerState* samplerState;
	ID3D11BlendState* transparencyBlendState;

	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*, WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, XMFLOAT4, XMFLOAT4, bool, bool picked, XMFLOAT4);

	void RenderShader(ID3D11DeviceContext*, int, int);

public:
	DeferredShaderHandler();
	~DeferredShaderHandler();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();

	bool Render(ID3D11DeviceContext*, int, int, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, XMFLOAT4, XMFLOAT4, bool, bool picked, XMFLOAT4);
};

#endif