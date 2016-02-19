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
	Vertex* vertices = nullptr;
	unsigned long* indices = nullptr;
	int size = 0;
	D3D11_BUFFER_DESC vertexBufferDesc;
	D3D11_BUFFER_DESC indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData;
	D3D11_SUBRESOURCE_DATA indexData;
	HRESULT hresult;
	bool result;

	result = this->LoadObj("../Ze3DProject/OBJ/Pyramid.obj", vertices, indices, size);
	if (!result) {
		return false;
	}
	//Set the number of vertices in the vertex array
	this->vertexCount = size;
	//Set the numer of indices in the index array
	this->indexCount = size;

	////SUBJECT TO CHANGE//
	////Set the number of vertices in the vertex array
	//this->vertexCount = 3;
	////Set the numer of indices in the index array
	//this->indexCount = 3;

	////Create the vertex array
	//vertices = new Vertex[this->vertexCount];
	//if (!vertices) {
	//	return false;
	//}

	////Create the index array
	//indices = new unsigned long[this->indexCount];
	//if (!indices) {
	//	return false;
	//}

	////Load the vertex array with data
	////Order is important, otherwise the triangle will be facing the opposite direction
	//vertices[0].position = XMFLOAT3(-1.0, -1.0f, 0.0f); //Bottom left
	//vertices[0].texture = XMFLOAT2(0.0f, 1.0f);

	//vertices[1].position = XMFLOAT3(0.0, 1.0f, 0.0f); //Top Middle
	//vertices[1].texture = XMFLOAT2(0.5f, 0.0f);

	//vertices[2].position = XMFLOAT3(1.0, -1.0f, 0.0f); //Bottom right
	//vertices[2].texture = XMFLOAT2(1.0f, 1.0f);

	////Load the index array with data
	//indices[0] = 0;	//Bottom left
	//indices[1] = 1;	//Top Middle
	//indices[2] = 2;	//Bottom right

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

bool Model::LoadObj(char* filename, Vertex*& outputVertices, unsigned long*& outputIndices, int& size)
{
	XMFLOAT3 tempVertex;
	XMFLOAT2 tempUV;
	XMFLOAT3 tempNormal;
	XMINT3 vertexIndex;
	XMINT3 uvIndex;
	XMINT3 normalIndex;
	std::vector<XMFLOAT3> tempVertices;
	std::vector<XMFLOAT2> tempUvs;
	std::vector<XMFLOAT3> tempNormals;
	std::string line;
	std::string junks;
	char junk;
	std::stringstream ss;
	std::ifstream file;

	file.open(filename);
	if (!file.is_open()) {
		return false;
	}

	while (std::getline(file, line)) {
		if (line.size() > 0) {
			if (line.at(0) == 'v') {
				if (line.at(1) == ' ') {
					ss.clear();
					ss.str(line);
					ss >> junk >> tempVertex.x >> tempVertex.y >> tempVertex.z;
					//sscanf_s(line.c_str(), "%f %f %f\n", &tempVertex.x, &tempVertex.y, &tempVertex.z);
					tempVertices.push_back(tempVertex);
				}
				else if (line.at(1) == 't') {
					ss.clear();
					ss.str(line);
					ss >> junks >> tempUV.x >> tempUV.y;
					//sscanf_s(line.c_str(), "%f %f\n", &tempUV.x, &tempUV.y);
					tempUvs.push_back(tempUV);
				}
				else if (line.at(1) == 'n') {
					ss.clear();
					ss.str(line);
					ss >> junks >> tempNormal.x >> tempNormal.y >> tempNormal.z;
					//sscanf_s(line.c_str(), "%f %f %f\n", &tempNormal.x, &tempNormal.y, &tempNormal.z);
					tempNormals.push_back(tempNormal);
				}
			}
			else if (line.at(0) == 'f') {
				ss.clear();
				ss.str(line);
				ss >> junk >> vertexIndex.x >> junk >> uvIndex.x >> junk >> normalIndex.x
					>> vertexIndex.y >> junk >> uvIndex.y >> junk >> normalIndex.y
					>> vertexIndex.z >> junk >> uvIndex.z >> junk >> normalIndex.z;
				/*int matches = sscanf_s(line.c_str(), "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex.x, &uvIndex.x, &normalIndex.x,
					&vertexIndex.y, &uvIndex.y, &normalIndex.y, &vertexIndex.z, &uvIndex.z, &normalIndex.z);
				if (matches != 9) {
					return false;
				}*/
				/*if (tempVertices.size() != tempUvs.size() || tempUvs.size() != tempNormals.size()) {
				return false;
				}*/
				if (!outputVertices) {
					size = tempVertices.size();
					outputVertices = new Vertex[size];
				}
				outputVertices[vertexIndex.x - 1].position = tempVertices.at(vertexIndex.x - 1);
				outputVertices[vertexIndex.x - 1].texture = tempUvs.at(vertexIndex.x - 1);
				outputVertices[vertexIndex.x - 1].normal = tempNormals.at(vertexIndex.x - 1);
			}
		}
		
	}
	outputIndices = new unsigned long[size];
	for (int i = 0; i < size; i++) {
		outputIndices[i] = i;
	}

	return true;
}