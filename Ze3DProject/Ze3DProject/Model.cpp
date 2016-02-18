#include "Model.h"


Model::Model() 
{
	this->vertexBuffer = nullptr;
	this->indexBuffer = nullptr;
	this->texture = nullptr;
}

Model::Model(const Model& originalObj)
{

}

Model::~Model() 
{

}

bool Model::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* textureFilename) 
{
	bool result;

	//Initialze the vertex and index buffer
	result = this->InitializeBuffers(device);
	if (!result) {
		return false;
	}

	//Load texture for this model
	result = this->LoadTexture(device, deviceContext, textureFilename);
	if (!result) {
		return false;
	}

	return true;
}

void Model::Shutdown() 
{
	//Release model texute
	this->ReleaseTexture();
	//Shutdown the vertex and index buffers
	this->ShutdownBuffers();

	return;
}

void Model::Render(ID3D11DeviceContext* deviceContext) 
{
	//Put the vertex and index buffers on the graphic pipeline to prepare them for drawing
	this->RenderBuffers(deviceContext);

	return;
}

int Model::GetIndexCount() 
{
	return this->indexCount;
}

ID3D11ShaderResourceView* Model::GetTexture()
{
	return this->texture->GetTexture();
}

bool Model::InitializeBuffers(ID3D11Device* device) 
{
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
	vertices[0].texture = XMFLOAT2(0.0f, 1.0f);

	vertices[1].position = XMFLOAT3(0.0, 1.0f, 0.0f); //Top Middle
	vertices[1].texture = XMFLOAT2(0.5f, 0.0f);

	vertices[2].position = XMFLOAT3(1.0, -1.0f, 0.0f); //Bottom right
	vertices[2].texture = XMFLOAT2(1.0f, 1.0f);

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

void Model::ShutdownBuffers() 
{
	//Release the index buffer
	if (this->indexBuffer) {
		this->indexBuffer->Release();
		this->indexBuffer = nullptr;
	}

	//Release the vertex buffer
	if (this->vertexBuffer) {
		this->vertexBuffer->Release();
		this->vertexBuffer = nullptr;
	}

	return;
}

void Model::RenderBuffers(ID3D11DeviceContext* deviceContext) 
{
	unsigned int stride;
	unsigned offset;

	//Set vertex buffer stride and offset
	stride = sizeof(Vertex);
	offset = 0;

	//Set the vertex buffer to active in the input assembly so it can rendered
	deviceContext->IASetVertexBuffers(0, 1, &this->vertexBuffer, &stride, &offset);

	//Set the index buffer to active in the input assembler so it can be rendered
	deviceContext->IASetIndexBuffer(this->indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	//Set the type od primitiv that should be rendered from this vertex buffer, in this case triangles
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

bool Model::LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename)
{
	bool result;

	//Create the texture object
	this->texture = new Texture;
	if (!this->texture) {
		return false;
	}

	//Init texture object
	result = this->texture->Initialize(device, deviceContext, filename);
	if (!result) {
		return false;
	}

	return true;
}

void Model::ReleaseTexture()
{
	//Release the texture object
	if (this->texture) {
		this->texture->Shutdown();
		delete this->texture;
		this->texture = nullptr;
	}

	return;
}

bool Model::LoadObj(char* filename)
{
	XMFLOAT3 tempVertex;
	XMFLOAT2 tempUV;
	XMFLOAT3 tempNormal;
	XMFLOAT3X3 tempFaces;
	std::vector<XMFLOAT3> vertices;
	std::vector<XMFLOAT2> uvs;
	std::vector<XMFLOAT3> normals;
	std::vector<XMFLOAT3X3> faces;
	std::string line;
	std::ifstream file;

	file.open(filename);

	while (std::getline(file, line)) {
		if (line.at(0) == 'v') {
			if (line.at(1) == ' ') {
				sscanf(line.c_str(), " %f %f %f ", &tempVertex.x, &tempVertex.y, &tempVertex.z);
				vertices.push_back(tempVertex);
			}
			else if (line.at(1) == 't') {
				sscanf(line.c_str(), " %f %f ", &tempUV.x, &tempUV.y);
				uvs.push_back(tempUV);
			}
			else if (line.at(1) == 'n') {
				sscanf(line.c_str(), " %f %f %f ", &tempNormal.x, &tempNormal.y, &tempNormal.z);
				normals.push_back(tempNormal);
			}
		}
		else if (line.at(0) == 'f') {
			sscanf(line.c_str(), " %f %f %f %f %f %f %f %f %f ", &tempFaces._11, &tempFaces._12, &tempFaces._13, 
				&tempFaces._21, &tempFaces._22, &tempFaces._23, &tempFaces._31, &tempFaces._32, &tempFaces._33);
			faces.push_back(tempFaces);
		}
	}

	return true;
}