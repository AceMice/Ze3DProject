#include "D3DHandler.h"

D3DHandler::D3DHandler()
{
	this->device = nullptr;
	this->deviceContext = nullptr;
	this->swapChain = nullptr;

	this->renderTargetView = nullptr;

	this->depthStencilBuffer = nullptr;
	this->depthStencilState = nullptr;
	this->disabledDepthStencilState = nullptr;
	this->depthStencilView = nullptr;
	this->rasterState = nullptr;
	
	for (int i = 0; i < BUFFER_COUNT; i++) {
		this->deferredRenderTargetTextures[i] = nullptr;
		this->deferredRenderTargetViews[i] = nullptr;
		this->deferredShaderResources[i] = nullptr;
	}

	this->shadowRenderTargetTexture = nullptr;
	this->shadowRenderTargetView = nullptr;
	this->shadowShaderResource = nullptr;

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
	D3D11_TEXTURE2D_DESC renderTextureDesc;
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;

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

	//Depth buffer desc
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	//Fill depth buffer description	
	depthBufferDesc.Width = screenWidth;
	depthBufferDesc.Height = screenHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	//Create depth texture
	hresult = this->device->CreateTexture2D(&depthBufferDesc, NULL, &this->depthStencilBuffer);
	if (FAILED(hresult)) {
		MessageBox(hwnd, L"this->device->CreateTexture2D", L"Error", MB_OK);
		return false;
	}

	//Depth stencil desc
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	//Fill depth stencil descrption
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	//Stencil operations if pixel is front-facing.
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	//Stencil operations if pixel is back-facing.
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	//Create the depth stencil state
	hresult = this->device->CreateDepthStencilState(&depthStencilDesc, &this->depthStencilState);
	if (FAILED(hresult)) {
		MessageBox(hwnd, L"this->device->CreateDepthStencilState", L"Error", MB_OK);
		return false;
	}

	//Set the newly created depth stencil
	this->deviceContext->OMSetDepthStencilState(this->depthStencilState, 1);

	////Depth stencil desc
	//ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	//Fill depth stencil descrption
	depthStencilDesc.DepthEnable = false;
	//depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	//depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	//depthStencilDesc.StencilEnable = true;
	//depthStencilDesc.StencilReadMask = 0xFF;
	//depthStencilDesc.StencilWriteMask = 0xFF;

	////Stencil operations if pixel is front-facing.
	//depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	//depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	//depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	//depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	////Stencil operations if pixel is back-facing.
	//depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	//depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	//depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	//depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	//Create the disabled depth stencil state
	hresult = this->device->CreateDepthStencilState(&depthStencilDesc, &this->disabledDepthStencilState);
	if (FAILED(hresult)) {
		MessageBox(hwnd, L"this->device->CreateDepthStencilState", L"Error", MB_OK);
		return false;
	}

	

	//Init depth stencil view description
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	//Fill the desc
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	//Create the depth stencil view
	hresult = this->device->CreateDepthStencilView(this->depthStencilBuffer, &depthStencilViewDesc, &this->depthStencilView);
	if (FAILED(hresult)) {
		MessageBox(hwnd, L"this->device->CreateDepthStencilView", L"Error", MB_OK);
		return false;
	}

	//Bind the render target view and depth stencil buffer to the output render pipeline
	this->deviceContext->OMSetRenderTargets(1, &this->renderTargetView, this->depthStencilView);

	//Setup the resterizer state for more rendering control
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_NONE;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	//Create the resterizer state from description
	hresult = this->device->CreateRasterizerState(&rasterDesc, &this->rasterState);
	if (FAILED(hresult)) {
		MessageBox(hwnd, L"this->device->CreateRasterizerState", L"Error", MB_OK);
		return false;
	}

	//Set the rasterizer state
	this->deviceContext->RSSetState(this->rasterState);

	//Setup the viewport
	viewport.Width = (float)screenWidth;
	viewport.Height = (float)screenHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	//Create the viewport
	this->deviceContext->RSSetViewports(1, &viewport);

	//Setup projection matrix
	//fieldOfView = 3.141592654f / 4.0f;
	fieldOfView = (float)XM_PI / 4.0f;
	screenAspect = (float)screenWidth / (float)screenHeight;

	this->projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth);

	//Create an orthographic projection matrix for 2D rendering
	this->orthoMatrix = XMMatrixOrthographicLH((float)screenWidth, (float)screenHeight, screenNear, screenDepth);

	//Initialize the render target texture description
	ZeroMemory(&renderTextureDesc, sizeof(renderTextureDesc));

	//Setup the render target texture description
	renderTextureDesc.Width = screenWidth;
	renderTextureDesc.Height = screenHeight;
	renderTextureDesc.MipLevels = 1;
	renderTextureDesc.ArraySize = 1;
	renderTextureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	renderTextureDesc.SampleDesc.Count = 1;
	renderTextureDesc.Usage = D3D11_USAGE_DEFAULT;
	renderTextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	renderTextureDesc.CPUAccessFlags = 0;
	renderTextureDesc.MiscFlags = 0;

	//Create the render target textures
	for (int i = 0; i < BUFFER_COUNT; i++) {
		hresult = this->device->CreateTexture2D(&renderTextureDesc, NULL, &this->deferredRenderTargetTextures[i]);
		if (FAILED(hresult)) {
			MessageBox(hwnd, L"device->CreateTexture2D", L"Error", MB_OK);
			return false;
		}
	}

	//Depth buffer desc
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	//Fill depth buffer description	
	depthBufferDesc.Width = 2048.0f;
	depthBufferDesc.Height = 2048.0f;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	hresult = this->device->CreateTexture2D(&depthBufferDesc, NULL, &this->shadowRenderTargetTexture);
	if (FAILED(hresult)) {
		MessageBox(hwnd, L"device->CreateTexture2DShadow", L"Error", MB_OK);
		return false;
	}

	depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;

	//Create the depth stencil view
	hresult = this->device->CreateDepthStencilView(this->shadowRenderTargetTexture, &depthStencilViewDesc, &this->shadowDepthStencilView);
	if (FAILED(hresult)) {
		MessageBox(hwnd, L"this->device->CreateDepthStencilView", L"Error", MB_OK);
		return false;
	}
	//Setup the description of the render target view.
	renderTargetViewDesc.Format = renderTextureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	//Create the render target views.
	for (i = 0; i<BUFFER_COUNT; i++) {
		hresult = device->CreateRenderTargetView(this->deferredRenderTargetTextures[i], &renderTargetViewDesc, &this->deferredRenderTargetViews[i]);
		if (FAILED(hresult)) {
			MessageBox(hwnd, L"device->CreateRenderTargetView", L"Error", MB_OK);
			return false;
		}
	}

	//hresult = device->CreateRenderTargetView(this->shadowRenderTargetTexture, &renderTargetViewDesc, &this->shadowRenderTargetView);
	//if (FAILED(hresult)) {
	//	MessageBox(hwnd, L"device->CreateRenderTargetView", L"Error", MB_OK);
	//	return false;
	//}

	// Setup the description of the shader resource view.
	shaderResourceViewDesc.Format = renderTextureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	//Create the shader resource views.
	for (int i = 0; i < BUFFER_COUNT; i++) {
		hresult = device->CreateShaderResourceView(this->deferredRenderTargetTextures[i], &shaderResourceViewDesc, &this->deferredShaderResources[i]);
		if (FAILED(hresult)) {
			MessageBox(hwnd, L"device->CreateShaderResourceView", L"Error", MB_OK);
			return false;
		}
	}

	shaderResourceViewDesc.Format = DXGI_FORMAT_R32_FLOAT;

	hresult = device->CreateShaderResourceView(this->shadowRenderTargetTexture, &shaderResourceViewDesc, &this->shadowShaderResource);
	if (FAILED(hresult)) {
		MessageBox(hwnd, L"device->CreateShaderResourceViewShadow", L"Error", MB_OK);
		return false;
	}

	return true;
}

void D3DHandler::Shutdown()
{
	this->deviceContext->Flush();
	//Set to window mode before shutting down or releasing the swapchain will throw and exception
	if (this->swapChain) {
		this->swapChain->SetFullscreenState(false, NULL);
	}

	if (this->rasterState) {
		this->rasterState->Release();
		this->rasterState = nullptr;
	}

	if (this->depthStencilView) {
		this->depthStencilView->Release();
		this->depthStencilView = nullptr;
	}

	if (this->depthStencilState) {
		this->depthStencilState->Release();
		this->depthStencilState = nullptr;
	}

	if (this->disabledDepthStencilState) {
		this->disabledDepthStencilState->Release();
		this->disabledDepthStencilState = nullptr;
	}

	if (this->depthStencilBuffer) {
		this->depthStencilBuffer->Release();
		this->depthStencilBuffer = nullptr;
	}

	if (this->renderTargetView) {
		this->renderTargetView->Release();
		this->renderTargetView = nullptr;
	}

	for (int i = 0; i < BUFFER_COUNT; i++) {
		if (this->deferredRenderTargetTextures[i]) {
			this->deferredRenderTargetTextures[i]->Release();
			this->deferredRenderTargetTextures[i] = nullptr;
		}
		if (this->deferredRenderTargetViews[i]) {
			this->deferredRenderTargetViews[i]->Release();
			this->deferredRenderTargetViews[i] = nullptr;
		}
		if (this->deferredShaderResources[i]) {
			this->deferredShaderResources[i]->Release();
			this->deferredShaderResources[i] = nullptr;
		}
	}

	if (this->shadowRenderTargetTexture) {
		this->shadowRenderTargetTexture->Release();
		this->shadowRenderTargetTexture = nullptr;
	}
	if (this->shadowRenderTargetView) {
		this->shadowRenderTargetView->Release();
		this->shadowRenderTargetView = nullptr;
	}
	if (this->shadowShaderResource) {
		this->shadowShaderResource->Release();
		this->shadowShaderResource = nullptr;
	}

	if (this->deviceContext) {
		this->deviceContext->Release();
		this->deviceContext = nullptr;
	}

	if (this->device) {
		this->device->Release();
		this->device = nullptr;
	}

	if (this->swapChain) {
		this->swapChain->Release();
		this->swapChain = nullptr;
	}

	return;
}

void D3DHandler::BeginScene(float red, float green, float blue, float alpha)
{
	float color[4];

	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;


	//Clear the back buffer
	this->deviceContext->ClearRenderTargetView(this->renderTargetView, color);

	//Clear the render target textures
	for (int i = 0; i < BUFFER_COUNT; i++) {
		this->deviceContext->ClearRenderTargetView(this->deferredRenderTargetViews[i], color);
	}

	//Clear the shadow map textures
	this->deviceContext->ClearRenderTargetView(this->shadowRenderTargetView, color);

	//Clear the depth buffer
	this->deviceContext->ClearDepthStencilView(this->depthStencilView, D3D11_CLEAR_DEPTH, 1.f, 0);

	//Clear the depth buffer
	this->deviceContext->ClearDepthStencilView(this->shadowDepthStencilView, D3D11_CLEAR_DEPTH, 1.f, 0);

	return;
}

void D3DHandler::EndScene()
{
	//Present the back buffer to the screen when rendering is complete
	if (this->vsync_enabled) {
		//Lock to refreshrate
		this->swapChain->Present(1, 0);
	}
	else {
		//Present asap
		this->swapChain->Present(0, 0);
	}

	return;
}

ID3D11Device* D3DHandler::GetDevice()
{
	return this->device;
}

ID3D11DeviceContext* D3DHandler::GetDeviceContext()
{
	return this->deviceContext;
}

void D3DHandler::GetProjectionMatrix(XMMATRIX& projectionMatrix)
{
	projectionMatrix = this->projectionMatrix;
	return;
}

void D3DHandler::GetOrthoMatrix(XMMATRIX& viewMatrix)
{
	viewMatrix = this->orthoMatrix;
	return;
}

void D3DHandler::GetVideoCardInfo(char* cardName, int& memory)
{
	strcpy_s(cardName, 128, this->videoCardDescription);
	memory = this->videoCardMemory;
	return;
}

void D3DHandler::ChangeRenderTargets(int renderTargetIndex) //1 Deferred, 2 ShadowMap, 3(else) BackBuffer
{
	if (renderTargetIndex == 1) {
		this->deviceContext->OMSetRenderTargets(BUFFER_COUNT, this->deferredRenderTargetViews, this->depthStencilView);
	}
	else if (renderTargetIndex == 2) {
		this->deviceContext->OMSetRenderTargets(0, NULL, this->shadowDepthStencilView);
	}
	else {
		this->deviceContext->OMSetRenderTargets(1, &this->renderTargetView, this->depthStencilView);
	}
}

ID3D11ShaderResourceView* D3DHandler::GetShaderResourceView(int resourceIndex)
{
	if (resourceIndex < BUFFER_COUNT) {
		return this->deferredShaderResources[resourceIndex];
	}
	else if (resourceIndex == BUFFER_COUNT) {
		return this->shadowShaderResource;
	}
	
	return NULL;
}

void D3DHandler::SetZBuffer(bool zBuffing)
{
	if (zBuffing) {
		this->deviceContext->OMSetDepthStencilState(this->depthStencilState, 1);
	}
	else {
		this->deviceContext->OMSetDepthStencilState(this->disabledDepthStencilState, 1);
	}
}

void D3DHandler::SetShadowViewport(bool shadowViewport)
{
	D3D11_VIEWPORT viewport;

	if (shadowViewport) {
		//Setup the viewport
		viewport.Width = 2048.0f;
		viewport.Height = 2048.0f;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;
	}
	else {
		viewport.Width = 800.0f;
		viewport.Height = 600.0f;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;
	}
	
	//Create the viewport
	this->deviceContext->RSSetViewports(1, &viewport);
}