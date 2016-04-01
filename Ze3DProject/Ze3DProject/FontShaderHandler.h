#ifndef FONTSHADERHANDLER_H
#define FONTSHADERHANDLER_H

#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <fstream>

using namespace DirectX;
using namespace std;

class FontShaderHandler {
private:
	struct MatrixBufferSimple
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
		XMFLOAT3 color;
	};

	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11InputLayout* layout;
	ID3D11Buffer* matrixBuffer;
	ID3D11SamplerState* samplerState;
	ID3D11BlendState* transparencyBlendState;

	bool InitializeShader(ID3D11Device*, WCHAR*, WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, XMFLOAT3);

	void RenderShader(ID3D11DeviceContext*, int);

public:
	FontShaderHandler();
	~FontShaderHandler();

	bool Initialize(ID3D11Device*);
	void Shutdown();

	bool Render(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, XMFLOAT3);
};

#endif