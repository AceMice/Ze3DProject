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

bool D3DHandler::Initialize(int screenWidth, int screenHeight, HWND hwnd, bool vsync, bool fullscreen, float screenDepth, float screenNear) 
{
	HRESULT hresult;
	IDXGIFactory* factory;
	IDXGIAdapter* adapter;
	IDXGIOutput* adapterOutput;
	unsigned int numModes, i, numerator, denominator;
	unsigned long long stringLength;
	DXGI_MODE_DESC* displayModeList;
	DXGI_ADAPTER_DESC adapterDesc;
	int error;
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	D3D_FEATURE_LEVEL featureLevel;
	ID3D11Texture2D* backBufferPtr;
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	D3D11_RASTERIZER_DESC rasterDesc;
	D3D11_VIEWPORT viewport;
	float fieldOfView, screenAspect;

	//Store the vsync setting
	this->vsync_enabled = vsync;

	//Create a DirectX graphics interface factory (Getting refresh rate from gpu/monitor ?)
	hresult = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if (FAILED(hresult)) {
		MessageBox(hwnd, L"CreateDXGIFactory", L"Error", MB_OK);
		return false;
	}
	//Use factory to create an adapter for the primary gpu
	hresult = factory->EnumAdapters(0, &adapter);
	if (FAILED(hresult)) {
		MessageBox(hwnd, L"factory->EnumAdapters", L"Error", MB_OK);
		return false;
	}
	//Emunerate the primary monitor
	hresult = adapter->EnumOutputs(0, &adapterOutput);
	if (FAILED(hresult)) {
		MessageBox(hwnd, L"adapter->EnumOutputs", L"Error", MB_OK);
		return false;
	}
	//Get the number of modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM display format for the monitor
	hresult = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	if (FAILED(hresult)) {
		MessageBox(hwnd, L"adapterOutput->GetDisplayModeList1", L"Error", MB_OK);
		return false;
	}
	//Create a list to store the possible display modes for given gpu/monitor combination
	displayModeList = new DXGI_MODE_DESC[numModes];
	if (!displayModeList) {
		MessageBox(hwnd, L"new DXGI_MODE_DESC[numModes]", L"Error", MB_OK);
		return false;
	}
	//Fill the display mode list struct
	hresult = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
	if (FAILED(hresult)) {
		MessageBox(hwnd, L"adapterOutput->GetDisplayModeList2", L"Error", MB_OK);
		return false;
	}
	//Loop through display modes and find the one matching the screen width and height
	for (int i = 0; i < numModes; i++) {
		if (displayModeList[i].Width == (unsigned int)screenWidth) {
			if (displayModeList[i].Height == (unsigned int)screenHeight) {
				numerator = displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}


}