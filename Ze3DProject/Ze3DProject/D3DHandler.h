#ifndef D3DHANDLER_H
#define D3DHANDLER_H

//Libirary linking
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include <windows.h>
#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;

class D3DHandler {
private:
	bool vsync_enabled;
	int videoCardMemory;
	char videoCardDescription[128];

	IDXGISwapChain* swapChain;
	ID3D11Device* device;
	ID3D11DeviceContext* deviceContext;
	ID3D11RenderTargetView* renderTargetView;
	ID3D11Texture2D* depthStencilBuffer;
	ID3D11DepthStencilState* depthStencilState;
	ID3D11DepthStencilView* depthStencilView;
	ID3D11RasterizerState* rasterState;
	XMMATRIX projectionMatrix;
	XMMATRIX orthoMatrix;

public:
	D3DHandler();
	~D3DHandler();

	bool Initialize(int, int, HWND, bool, bool, float, float);
	void Shutdown();

	void BeginScene(float, float, float, float);
	void EndScene();

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();

	void GetProjectionMatrix(XMMATRIX&);
	void GetOrthoMatrix(XMMATRIX&);

	void GetVideoCardInfo(char*, int&);
};

#endif