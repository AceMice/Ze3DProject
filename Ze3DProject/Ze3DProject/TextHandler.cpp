#include "TextHandler.h"


TextHandler::TextHandler()
{
	this->baseViewMatrix = XMMatrixIdentity();
	this->font = nullptr;
	this->fontShaderH = nullptr;
}

TextHandler::TextHandler(const TextHandler& originalObj)
{

}

TextHandler::~TextHandler()
{

}

int TextHandler::CreateSentence(ID3D11Device* device, int maxLength)
{
	D3D11_BUFFER_DESC vertexBufferDesc;
	D3D11_BUFFER_DESC indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData;
	D3D11_SUBRESOURCE_DATA indexData;
	HRESULT hresult;
	bool result;
	Sentence* tempSentence = new Sentence;

	tempSentence->maxLength = maxLength;
	tempSentence->vertexCount = tempSentence->indexCount = maxLength * 6;

	Vertex* vertices = new Vertex[tempSentence->vertexCount];
	memset(vertices, 0, (sizeof(Vertex) * tempSentence->vertexCount));

	unsigned long* indices = new unsigned long[tempSentence->indexCount];
	for (int i = 0; i < tempSentence->indexCount; i++) {
		indices[i] = i;
	}

	//Set the description of the static vertex buffer
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * tempSentence->vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	//Give the subresource structure a pointer to the vertex data
	ZeroMemory(&vertexData, sizeof(vertexData));
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	//Create the vertex buffer
	hresult = device->CreateBuffer(&vertexBufferDesc, &vertexData, &tempSentence->vertexBuffer);
	if (FAILED(hresult)) {
		return -1;
	}

	//Set up the description of the static index buffer
	ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * tempSentence->indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	ZeroMemory(&indexData, sizeof(indexData));
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	//Create the index buffer
	hresult = device->CreateBuffer(&indexBufferDesc, &indexData, &tempSentence->indexBuffer);
	if (FAILED(hresult)) {
		return -1;
	}

	this->sentences.push_back(tempSentence);

	delete[] vertices;
	delete[] indices;

	return this->sentences.size() - 1;
}

bool TextHandler::UpdateSentence(ID3D11DeviceContext* deviceContext, int id, std::string text, int posX, int posY, XMFLOAT3 color)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	Sentence* tempSentence = this->sentences.at(id);

	int nrLetters = text.size();
	if (nrLetters > tempSentence->maxLength) {
		return false;
	}

	Vertex* vertices = new Vertex[tempSentence->vertexCount];
	memset(vertices, 0, (sizeof(Vertex) * tempSentence->vertexCount));

	float drawPosX = ((this->screenWidth / 2) * -1) + posX;
	float drawPosY = (this->screenHeight / 2) - posY;

	this->font->BuildVertexArray((void*)vertices, text.c_str(), drawPosX, drawPosY);

	result = deviceContext->Map(tempSentence->vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	Vertex* verticesPtr = (Vertex*)mappedResource.pData;

	//Copy the new vertex data to buffer
	memcpy(verticesPtr, vertices, (sizeof(Vertex) * tempSentence->vertexCount));

	deviceContext->Unmap(tempSentence->vertexBuffer, 0);

	delete[] vertices;

	tempSentence->color = color;

	return true;
}

void TextHandler::ReleaseSentences()
{
	for (int i = 0; i < this->sentences.size(); i++) {
		if (this->sentences.at(i)) {
			delete this->sentences.at(i);
			this->sentences.at(i) = nullptr;
		}
	}

	return;
}

void TextHandler::RenderSentence(ID3D11DeviceContext* deviceContext, int id)
{
	Sentence* tempSentence = this->sentences.at(id);
	unsigned int stride;
	unsigned offset;

	//Set vertex buffer stride and offset
	stride = sizeof(Vertex);
	offset = 0;

	//Set the vertex buffer to active in the input assembly so it can rendered
	deviceContext->IASetVertexBuffers(0, 1, &tempSentence->vertexBuffer, &stride, &offset);

	//Set the index buffer to active in the input assembler so it can be rendered
	deviceContext->IASetIndexBuffer(tempSentence->indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	//Set the type od primitiv that should be rendered from this vertex buffer, in this case triangles
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

bool TextHandler::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, XMMATRIX baseViewMatrix, int screenWidth, int screenHeight)
{
	bool result;

	this->font = new Font;
	if (!this->font) {
		return false;
	}
	result = this->font->Initialize(device, deviceContext, "fontData.txt", "font.mtl");
	if (!result) {
		return false;
	}

	this->fontShaderH = new FontShaderHandler;
	if (!this->fontShaderH) {
		return false;
	}
	result = this->fontShaderH->Initialize(device);
	if (!result) {
		return false;
	}

	this->baseViewMatrix = baseViewMatrix;
	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;

	return true;
}

void TextHandler::Shutdown()
{
	this->ReleaseSentences();

	if (this->font) {
		this->font->Shutdown();
		delete this->font;
		this->font = nullptr;
	}

	return;
}

void TextHandler::Render(ID3D11DeviceContext* deviceContext, XMMATRIX orthoMatrix)
{
	Sentence* tempSentence;
	for (int i = 0; i < this->sentences.size(); i++) {
		tempSentence = this->sentences.at(i);
		this->RenderSentence(deviceContext, i);

		this->fontShaderH->Render(deviceContext, tempSentence->indexCount, XMMatrixIdentity(), this->baseViewMatrix, orthoMatrix, this->font->GetTexture(), tempSentence->color);
	}

	return;
}