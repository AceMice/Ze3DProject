#include "ModelHandler.h"


ModelHandler::ModelHandler() {
	this->vertexBuffer = nullptr;
	this->indexBuffer = nullptr;

}

ModelHandler::ModelHandler(const ModelHandler& originalObj) {

}

ModelHandler::~ModelHandler() {

}

bool ModelHandler::Initialize(ID3D11Device* device) {
	bool result;

	//Initialze the vertex and index buffer
	result = this->InitializeBuffers(device);
	if (!result) {
		return false;
	}

	return true;
}

void ModelHandler::Shutdown() {
	//Shutdown the vertex and index buffers
	this->Shutdown();

	return;
}

void ModelHandler::Render(ID3D11DeviceContext* deviceContext) {
	//Put the vertex and index buffers on the graphic pipeline to prepare them for drawing
	this->RenderBuffers(deviceContext);

	return;
}

int ModelHandler::GetIndexCount() {
	return this->indexCount;
}

bool ModelHandler::InitializeBuffers(ID3D11Device* device) {
	Vertex* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc;
	D3D11_BUFFER_DESC indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData;
	D3D11_SUBRESOURCE_DATA indexData;
	HRESULT hresult;

	//SUBJECT TO CHANGE//
	//Set the number of vertices in the vertex array
	this->vertexCount = 3;
	//Set the numer of indices in the index array
	this->indexCount = 3;

	//Create the vertex array
	vertices = new Vertex[this->vertexCount];
	if (!vertices) {
		return false;
	}

	//Create the index array
	indices = new unsigned long[this->indexCount];
	if (!indices) {
		return false;
	}

	//Load the vertex array with data
	//Order is important, otherwise the triangle will be facing the opposite direction
	vertices[0].position = XMFLOAT3(-1.0, -1.0f, 0.0f); //Bottom left
	vertices[0].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

	vertices[0].position = XMFLOAT3(0.0, 1.0f, 0.0f); //Top Middle
	vertices[0].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

	vertices[0].position = XMFLOAT3(1.0, -1.0f, 0.0f); //Bottom right
	vertices[0].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

	//Load the index array with data
	indices[0] = 0;	//Bottom left
	indices[1] = 1;	//Top Middle
	indices[2] = 2;	//Bottom right

	//Set the description of the static vertex buffer
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * this->vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	//Give the subresource structure a pointer to the vertex data
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	//Create the vertex buffer
	hresult = device->CreateBuffer(&vertexBufferDesc, &vertexData, &this->vertexBuffer);
	if (FAILED(hresult)) {
		return false;
	}

	//Set up the description of the static index buffer
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * this->indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	//Create the index buffer
	hresult = device->CreateBuffer(&indexBufferDesc, &indexData, &this->indexBuffer);
	if (FAILED(hresult)) {
		return false;
	}

	//Release the arrays now that the vertex and index buffers ave been created and loaded
	delete[] vertices;
	vertices = nullptr;
	delete[] indices;
	indices = nullptr;

	return true;
}

void ModelHandler::ShutdownBuffers() {

}

void ModelHandler::RenderBuffers(ID3D11DeviceContext*) {

}
