#include "GroundModel.h"


GroundModel::GroundModel()
{
	this->vertexBuffer = nullptr;
	this->indexBuffer = nullptr;
	this->texture = nullptr;
	this->worldMatrix = XMMatrixIdentity();
}

GroundModel::GroundModel(const GroundModel& originalObj)
{

}

GroundModel::~GroundModel()
{

}

bool GroundModel::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* bmpFile)
{
	bool result;
	std::string materialLib;

	this->name = bmpFile;

	//Initialze the vertex and index buffer
	result = this->InitializeBuffers(device, bmpFile, materialLib);
	if (!result) {
		return false;
	}

	//Load texture for this model
	result = this->LoadTexture(device, deviceContext, materialLib);
	if (!result) {
		return false;
	}

	return true;
}

void GroundModel::Shutdown()
{
	//Release model texute
	this->ReleaseTexture();
	//Shutdown the vertex and index buffers
	this->ShutdownBuffers();

	return;
}

void GroundModel::Render(ID3D11DeviceContext* deviceContext)
{
	//Put the vertex and index buffers on the graphic pipeline to prepare them for drawing
	this->RenderBuffers(deviceContext);

	return;
}

int GroundModel::GetIndexCount()
{
	return this->indexCount;
}

ID3D11ShaderResourceView* GroundModel::GetTexture(int textureIndex)
{
	return this->texture->GetTexture(textureIndex);
}

bool GroundModel::InitializeBuffers(ID3D11Device* device, char* bmpFile, std::string& materialName)
{
	std::vector<Vertex> vertices;
	unsigned long* indices = nullptr;
	int sizeVertices = 0;
	int sizeIndices = 0;
	D3D11_BUFFER_DESC vertexBufferDesc;
	D3D11_BUFFER_DESC indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData;
	D3D11_SUBRESOURCE_DATA indexData;
	HRESULT hresult;
	bool result;
	materialName = "ground.mtl";

	//Hard coded the material to be "ground"
	result = this->GenerateGround(bmpFile, materialName, vertices, indices, sizeVertices, sizeIndices);
	
	if (!result) {
		return false;
	}
	//Set the number of vertices in the vertex array
	this->vertexCount = sizeVertices;
	//Set the numer of indices in the index array
	this->indexCount = sizeIndices;

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
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * this->vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	//Give the subresource structure a pointer to the vertex data
	ZeroMemory(&vertexData, sizeof(vertexData));
	vertexData.pSysMem = &(vertices[0]);
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	//Create the vertex buffer
	hresult = device->CreateBuffer(&vertexBufferDesc, &vertexData, &this->vertexBuffer);
	if (FAILED(hresult)) {
		return false;
	}

	//Set up the description of the static index buffer
	ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * this->indexCount;	// IndexCount = numFaces*3
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	ZeroMemory(&indexData, sizeof(indexData));
	indexData.pSysMem = &(indices[0]);
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	//Create the index buffer
	hresult = device->CreateBuffer(&indexBufferDesc, &indexData, &this->indexBuffer);
	if (FAILED(hresult)) {
		return false;
	}

	//Release the arrays now that the vertex and index buffers ave been created and loaded
	delete[] indices;	//Never cleaned up
	indices = nullptr;
	vertices.clear();

	return true;
}

void GroundModel::ShutdownBuffers()
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

void GroundModel::RenderBuffers(ID3D11DeviceContext* deviceContext)
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

bool GroundModel::LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, std::string materialLib)
{
	bool result;

	//Create the texture object
	this->texture = new Texture;
	if (!this->texture) {
		return false;
	}

	//Init texture object
	result = this->texture->Initialize(device, deviceContext, materialLib);
	if (!result) {
		return false;
	}

	return true;
}

void GroundModel::ReleaseTexture()
{
	//Release the texture object
	if (this->texture) {
		this->texture->Shutdown();
		delete this->texture;
		this->texture = nullptr;
	}

	return;
}

bool GroundModel::GenerateGround(char* bmpFile, std::string matrialLib, std::vector<Vertex>& outputVertices, unsigned long*& outputIndices, int& sizeVertices, int& sizeIndices) {
	HeightMap hmObj;
	bool result = hmObj.HeightMapLoad(bmpFile, hmInfo);
	
	if (result != true) {
			return false;
	}
	int cols = hmInfo.terrainWidth;
	int rows = hmInfo.terrainHeight;

	//Create the grid
	int NumVertices = rows * cols;
	int NumFaces = (rows - 1)*(cols - 1) * 2;

	std::vector<Vertex> v(NumVertices);

	//Give each vertex a position and a normal
	for (DWORD i = 0; i < rows; ++i)
	{
		for (DWORD j = 0; j < cols; ++j)
		{
			v[i*cols + j].position = hmInfo.heightMap[i*cols + j];
			v[i*cols + j].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
		}
	}

	//Create Index list and UV cords
	std::vector<DWORD> indices(NumFaces * 3);
	int k = 0;
	int texUIndex = 0;
	int texVIndex = 0;
	for (unsigned long i = 0; i < rows - 1; i++)
	{
		for (unsigned long j = 0; j < cols - 1; j++)
		{
			indices[k] = i*cols + j;        // Bottom left of quad
			v[i*cols + j].texture = XMFLOAT2(texUIndex + 0.0f, texVIndex + 1.0f);

			indices[k + 1] = i*cols + j + 1;        // Bottom right of quad
			v[i*cols + j + 1].texture = XMFLOAT2(texUIndex + 1.0f, texVIndex + 1.0f);

			indices[k + 2] = (i + 1)*cols + j;    // Top left of quad
			v[(i + 1)*cols + j].texture = XMFLOAT2(texUIndex + 0.0f, texVIndex + 0.0f);


			indices[k + 3] = (i + 1)*cols + j;    // Top left of quad
			v[(i + 1)*cols + j].texture = XMFLOAT2(texUIndex + 0.0f, texVIndex + 0.0f);

			indices[k + 4] = i*cols + j + 1;        // Bottom right of quad
			v[i*cols + j + 1].texture = XMFLOAT2(texUIndex + 1.0f, texVIndex + 1.0f);

			indices[k + 5] = (i + 1)*cols + j + 1;    // Top right of quad
			v[(i + 1)*cols + j + 1].texture = XMFLOAT2(texUIndex + 1.0f, texVIndex + 0.0f);

			k += 6; // next quad

			texUIndex++;
		}
		texUIndex = 0;
		texVIndex++;
	}

	//////////////////////Compute Normals///////////////////////////
	//Now we will compute the normals for each vertex using normal averaging
	std::vector<XMFLOAT3> tempNormal;

	//normalized and unnormalized normals
	XMFLOAT3 unnormalized = XMFLOAT3(0.0f, 0.0f, 0.0f);

	//Used to get vectors (sides) from the position of the verts
	float vecX, vecY, vecZ;

	//Two edges of our triangle
	XMVECTOR edge1 = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR edge2 = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

	//Compute face normals
	for (int i = 0; i < NumFaces; ++i)
	{
		//Get the vector describing one edge of our triangle (edge 0,2)
		vecX = v[indices[(i * 3)]].position.x - v[indices[(i * 3) + 2]].position.x;
		vecY = v[indices[(i * 3)]].position.y - v[indices[(i * 3) + 2]].position.y;
		vecZ = v[indices[(i * 3)]].position.z - v[indices[(i * 3) + 2]].position.z;
		edge1 = XMVectorSet(vecX, vecY, vecZ, 0.0f);    //Create our first edge

														//Get the vector describing another edge of our triangle (edge 2,1)
		vecX = v[indices[(i * 3) + 2]].position.x - v[indices[(i * 3) + 1]].position.x;
		vecY = v[indices[(i * 3) + 2]].position.y - v[indices[(i * 3) + 1]].position.y;
		vecZ = v[indices[(i * 3) + 2]].position.z - v[indices[(i * 3) + 1]].position.z;
		edge2 = XMVectorSet(vecX, vecY, vecZ, 0.0f);    //Create our second edge

														//Cross multiply the two edge vectors to get the un-normalized face normal
		XMStoreFloat3(&unnormalized, XMVector3Cross(edge1, edge2));
		tempNormal.push_back(unnormalized);            //Save unormalized normal (for normal averaging)
	}

	//Compute vertex normals (normal Averaging)
	XMVECTOR normalSum = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	int facesUsing = 0;
	float tX;
	float tY;
	float tZ;

	//Go through each vertex
	for (int i = 0; i < NumVertices; ++i)
	{
		//Check which triangles use this vertex
		for (int j = 0; j < NumFaces; ++j)
		{
			if (indices[j * 3] == i ||
				indices[(j * 3) + 1] == i ||
				indices[(j * 3) + 2] == i)
			{
				tX = XMVectorGetX(normalSum) + tempNormal[j].x;
				tY = XMVectorGetY(normalSum) + tempNormal[j].y;
				tZ = XMVectorGetZ(normalSum) + tempNormal[j].z;

				normalSum = XMVectorSet(tX, tY, tZ, 0.0f);    //If a face is using the vertex, add the unormalized face normal to the normalSum
				facesUsing++;
			}
		}

		//Get the actual normal by dividing the normalSum by the number of faces sharing the vertex
		normalSum = normalSum / facesUsing;

		//Normalize the normalSum vector
		normalSum = XMVector3Normalize(normalSum);

		//Store the normal in our current vertex
		v[i].normal.x = XMVectorGetX(normalSum);
		v[i].normal.y = XMVectorGetY(normalSum);
		v[i].normal.z = XMVectorGetZ(normalSum);

		//Clear normalSum and facesUsing for next vertex
		normalSum = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		facesUsing = 0;
	}
	//outputVertices = v;
	outputVertices = std::vector<Vertex>(v.size());

	sizeVertices = v.size();
	sizeIndices = indices.size();

	outputIndices = new unsigned long[sizeIndices];
	for (int i = 0; i < v.size(); i++) {
		outputVertices.at(i) = v.at(i);
	}

	for (int i = 0; i < sizeIndices; i++) {
		outputIndices[i] = indices.at(i);
	}

	this->materialNames.push_back("Grass");	//One material
	this->subsetIndices.push_back(0);
	return true;
}

void GroundModel::GetWorldMatrix(XMMATRIX& worldMatrix)
{
	worldMatrix = this->worldMatrix;

	return;
}

void GroundModel::SetWorldMatrix(XMMATRIX worldMatrix)
{
	this->worldMatrix = worldMatrix;
	return;
}

int GroundModel::NrOfSubsets()
{
	return this->subsetIndices.size();
}

void GroundModel::GetSubsetInfo(int subsetIndex, int& indexStart, int& indexCount, int& textureIndex, int& normMapIndex, XMFLOAT4& difColor, XMFLOAT4& specColor, bool& transparent)
{
	indexStart = this->subsetIndices.at(subsetIndex);

	if (this->subsetIndices.size() > subsetIndex + 1) {
		indexCount = this->subsetIndices.at(subsetIndex + 1) - indexStart;
	}
	else {
		indexCount = this->GetIndexCount() - indexStart;
	}
	Texture::Material tempMaterial = this->texture->GetMaterial(this->materialNames.at(subsetIndex));

	if (tempMaterial.hasTexture) {
		textureIndex = tempMaterial.textureIndex;
	}
	else {
		textureIndex = -1;
	}

	if (tempMaterial.hasNormMap) {
		normMapIndex = tempMaterial.normMapIndex;
	}
	else {
		normMapIndex = -1;
	}

	difColor = tempMaterial.difColor;
	specColor = tempMaterial.specColor;
	transparent = tempMaterial.transparent;

	return;
}

std::string GroundModel::GetName()
{
	return this->name;
}

HeightMap::HeightMapInfo GroundModel::getHeightMapInfo() {
	return this->hmInfo;
}