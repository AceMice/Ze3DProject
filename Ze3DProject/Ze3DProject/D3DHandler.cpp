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
	//When found, store numerator and denominator of refresh rate
	for (int i = 0; i < numModes; i++) {
		if (displayModeList[i].Width == (unsigned int)screenWidth) {
			if (displayModeList[i].Height == (unsigned int)screenHeight) {
				numerator = displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}

	//Get the adapter(gpu) description
	hresult = adapter->GetDesc(&adapterDesc);
	if (FAILED(hresult)) {
		MessageBox(hwnd, L"adapter->GetDesc", L"Error", MB_OK);
		return false;
	}
	//Store the dedicated video card memory in megabytes
	this->videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);
	//Store the video card name in char array
	error = wcstombs_s(&stringLength, this->videoCardDescription, 128, adapterDesc.Description, 128);
	if (error != 0) {
		MessageBox(hwnd, L"wcstombs_s", L"Error", MB_OK);
		return false;
	}

	//Release the display mode list
	delete[] displayModeList;
	displayModeList = nullptr;
	//Release the adapter output
	adapterOutput->Release();
	adapterOutput = nullptr;
	//Release the adapter
	adapter->Release();
	adapter = nullptr;
	//Release the factory
	factory->Release();
	factory = nullptr;

	//Init the swap chain description
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
	//Set to a single back buffer
	swapChainDesc.BufferCount = 1;
	//Set the width and height of back buffer
	swapChainDesc.BufferDesc.Width = screenWidth;
	swapChainDesc.BufferDesc.Height = screenHeight;
	//Set regular 32-bit suface for the back buffer
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	//Set the refresh rate of back buffer
	if (this->vsync_enabled) {
		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else {
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	//Set the usage of back buffer
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	//Set the handle of window to render in
	swapChainDesc.OutputWindow = hwnd;

	//Turn multisampling off
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	//Set to either fullscreen of window mode
	if (fullscreen) {
		swapChainDesc.Windowed = false;
	}
	else {
		swapChainDesc.Windowed = true;
	}

	//Set the scan line ordering and scaling to unspecified
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	//Discard the back buffer content after displaying
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	//No advanced flags
	swapChainDesc.Flags = 0;

	//Set the feature level to DirectX 11
	featureLevel = D3D_FEATURE_LEVEL_11_0;

	//Create the swap chain, Direct3D device and Direct3D device context
	hresult = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1, 
		D3D11_SDK_VERSION, &swapChainDesc, &this->swapChain, &this->device, NULL, &this->deviceContext);
	if (FAILED(hresult)) {
		MessageBox(hwnd, L"D3D11CreateDeviceAndSwapChain", L"Error", MB_OK);
		return false;
	}

	//Get the pointer to the back buffer
	hresult = this->swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
	if (FAILED(hresult)) {
		MessageBox(hwnd, L"this->swapChain->GetBuffer", L"Error", MB_OK);
		return false;
	}
	//Create the render target view with the back buffer pointer
	hresult = this->device->CreateRenderTargetView(backBufferPtr, NULL, &this->renderTargetView);
	if (FAILED(hresult)) {
		MessageBox(hwnd, L"this->device->CreateRenderTargetView", L"Error", MB_OK);
		return false;
	}
	//Release pointer to back buffer as it is no longer needed
	backBufferPtr->Release();
	backBufferPtr = nullptr;

	//Continue with the depth stecil

	return true;
}