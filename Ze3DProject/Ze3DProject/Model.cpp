#include "Model.h"


Model::Model() 
{
	this->vertexBuffer = nullptr;
	this->indexBuffer = nullptr;
	this->texture = nullptr;
	this->worldMatrix = XMMatrixIdentity();
}

Model::Model(const Model& originalObj)
{

}

Model::~Model() 
{

}

bool Model::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* modelFilename) 
{
	bool result;
	std::string materialLib;

	//Initialze the vertex and index buffer
	result = this->InitializeBuffers(device, modelFilename, materialLib);
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

ID3D11ShaderResourceView* Model::GetTexture(int textureIndex)
{
	return this->texture->GetTexture(textureIndex);
}

bool Model::InitializeBuffers(ID3D11Device* device, char* modelFilename, std::string& materialName)
{
	std::vector<Vertex>* vertices = new std::vector<Vertex>;
	unsigned long* indices = nullptr;
	int sizeVertices = 0;
	int sizeIndices = 0;
	D3D11_BUFFER_DESC vertexBufferDesc;
	D3D11_BUFFER_DESC indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData;
	D3D11_SUBRESOURCE_DATA indexData;
	HRESULT hresult;
	bool result;
	std::string path = "../Ze3DProject/OBJ/";
	/*std::string format = ".obj";
	std::string bin = ".bin";
	std::string finalBinPath = path + modelFilename + bin;*/
	std::string finalPath = path + modelFilename;
	result = this->LoadObj(finalPath.c_str(), vertices, indices, sizeVertices, sizeIndices, materialName);
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
	vertexData.pSysMem = &((*vertices)[0]);
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
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * this->indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	ZeroMemory(&indexData, sizeof(indexData));
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	//Create the index buffer
	hresult = device->CreateBuffer(&indexBufferDesc, &indexData, &this->indexBuffer);
	if (FAILED(hresult)) {
		return false;
	}

	//Release the arrays now that the vertex and index buffers ave been created and loaded
	delete[] indices;
	indices = nullptr;
	delete vertices;
	vertices = nullptr;

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

bool Model::LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, std::string materialLib)
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

bool Model::LoadObj(const char* filename, std::vector<Vertex>* outputVertices, unsigned long*& outputIndices, int& sizeVertices, int& sizeIndices, std::string& materialLib)
{
	XMFLOAT3 tempVertex;
	XMFLOAT2 tempUV;
	XMFLOAT3 tempNormal;
	unsigned int vertexIndex[3];
	unsigned int uvIndex[3];
	unsigned int normalIndex[3];
	std::vector<XMFLOAT3> tempVertices;
	std::vector<XMFLOAT2> tempUvs;
	std::vector<XMFLOAT3> tempNormals;
	std::vector<unsigned int> vertexIndices;
	std::vector<unsigned int> uvIndices;
	std::vector<unsigned int> normalIndices;
	std::string line;
	std::string junks;
	std::string tempLine;
	char junk;
	std::stringstream ss;
	std::fstream file;
	bool newGroup = false;
	int tempSubset = 0;
	std::string format = ".bin";
	std::string path = filename;
	path.append("ACE.txt");
	file.open(path, std::ios::in);
	if (file.is_open()) {
		std::getline(file, line);
		ss.clear();
		ss.str(line);
		ss >> sizeVertices >> sizeIndices;
		std::getline(file, line);
		ss.clear();
		ss.str(line);
		ss >> materialLib;
		while (std::getline(file, line)) {
			ss.clear();
			ss.str(line);
			ss >> tempSubset >> tempLine;
			subsetIndices.push_back(tempSubset);
			materialNames.push_back(tempLine);
		}
		file.close();
		path = filename;
		path.append(".bin");
		file.open(path, std::ios::binary | std::ios::in);
		if (!file.is_open()) {
			return false;
		}
		Vertex* tempVerticesArray = new Vertex[sizeVertices];

		file.read((char*)tempVerticesArray, sizeof(Vertex) * sizeVertices);
		file.close();

		outputVertices->insert(outputVertices->end(), &tempVerticesArray[0], &tempVerticesArray[sizeVertices]);

		delete[] tempVerticesArray;

		outputIndices = new unsigned long[sizeIndices];
		for (int i = 0; i < sizeIndices; i++) {
			outputIndices[i] = i;
		}
	}
	else {
		format = ".obj";
		path = filename + format;
		file.open(path, std::ios::in);
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
						tempUV.y = 1.0f - tempUV.y;
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
				else if (line.at(0) == 'g') {
					this->subsetIndices.push_back(vertexIndices.size());
					newGroup = true;
				}
				else if (line.substr(0, 6) == "usemtl") {
					ss.clear();
					ss.str(line);
					ss >> junks >> tempLine;
					this->materialNames.push_back(tempLine);
					if (!newGroup) {
						this->subsetIndices.push_back(vertexIndices.size());
					}
					else {
						newGroup = false;
					}
				}
				else if (line.at(0) == 'f') {
					if (newGroup) {
						this->materialNames.push_back(this->materialNames.back());
						newGroup = false;
					}
					ss.clear();
					ss.str(line);
					ss >> junk >> vertexIndex[0] >> junk >> uvIndex[0] >> junk >> normalIndex[0]
						>> vertexIndex[1] >> junk >> uvIndex[1] >> junk >> normalIndex[1]
						>> vertexIndex[2] >> junk >> uvIndex[2] >> junk >> normalIndex[2];

					/*int matches = sscanf_s(line.c_str(), "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex.x, &uvIndex.x, &normalIndex.x,
					&vertexIndex.y, &uvIndex.y, &normalIndex.y, &vertexIndex.z, &uvIndex.z, &normalIndex.z);
					if (matches != 9) {
					return false;
					}*/
					/*if (tempVertices.size() != tempUvs.size() || tempUvs.size() != tempNormals.size()) {
					return false;
					}*/
					vertexIndices.push_back(vertexIndex[0]);
					vertexIndices.push_back(vertexIndex[1]);
					vertexIndices.push_back(vertexIndex[2]);
					uvIndices.push_back(uvIndex[0]);
					uvIndices.push_back(uvIndex[1]);
					uvIndices.push_back(uvIndex[2]);
					normalIndices.push_back(normalIndex[0]);
					normalIndices.push_back(normalIndex[1]);
					normalIndices.push_back(normalIndex[2]);
				}
				else if (line.substr(0, 6) == "mtllib") {
					ss.clear();
					ss.str(line);
					ss >> junks >> materialLib;
				}
			}
		}
		file.close();

		//outputVertices[vertexIndex.x - 1].position = tempVertices.at(vertexIndex.x - 1);
		//outputVertices[vertexIndex.x - 1].texture = tempUvs.at(vertexIndex.x - 1);
		////outputVertices[vertexIndex.x - 1].normal = tempNormals.at(vertexIndex.x - 1);
		//outputVertices[vertexIndex.y - 1].position = tempVertices.at(vertexIndex.y - 1);
		//outputVertices[vertexIndex.y - 1].texture = tempUvs.at(vertexIndex.y - 1);

		//outputVertices[vertexIndex.z - 1].position = tempVertices.at(vertexIndex.z - 1);
		//outputVertices[vertexIndex.z - 1].texture = tempUvs.at(vertexIndex.z - 1);

		if (vertexIndices.size() == 0 || tempVertices.size() == 0) {
			return false;
		}

		sizeVertices = vertexIndices.size();
		sizeIndices = vertexIndices.size();
		outputIndices = new unsigned long[sizeIndices];

		/*for (int i = 0; i < sizeIndices; i++) {
		outputVertices[i].position = tempVertices.at(vertexIndices.at(i) - 1);
		outputVertices[i].texture = tempUvs.at(uvIndices.at(i) - 1);
		outputVertices[i].normal = tempNormals.at(normalIndices.at(i) - 1);
		}*/
		//for (int i = 0; i < this->subsetIndices.size(); i++) {
		//	for (int j = 0; j < this->subsetIndices.at(i); j++) {
		//		Vertex tempVertex;
		//		int position = ((i - 1) * this->subsetIndices.size()) + j;
		//		tempVertex.position = tempVertices.at(vertexIndices.at(position) - 1);
		//		tempVertex.texture = tempUvs.at(uvIndices.at(position) - 1);
		//		tempVertex.normal = tempNormals.at(normalIndices.at(position) - 1);
		//		outputVertices->push_back(tempVertex);
		//	}
		//}
		for (int i = 0; i < sizeVertices; i++) {
			Vertex tempVertex;
			tempVertex.position = tempVertices.at(vertexIndices.at(i) - 1);
			tempVertex.texture = tempUvs.at(uvIndices.at(i) - 1);
			tempVertex.normal = tempNormals.at(normalIndices.at(i) - 1);
			outputVertices->push_back(tempVertex);

			outputIndices[i] = i;
		}
		path = filename;
		path.append("ACE.txt");
		file.open(path, std::ios::out);
		if (!file.is_open()) {
			return false;
		}
		file << sizeVertices << " " << sizeIndices << "\n";
		file << materialLib << "\n";
		for (int i = 0; i < subsetIndices.size(); i++) {
			file << subsetIndices.at(i) << " " << materialNames.at(i) << "\n";
		}
		file.close();

		format = ".bin";
		path = filename + format;
		file.open(path, std::ios::out | std::ios::binary);
		if (!file.is_open()) {
			return false;
		}
		file.write((char*)&((*outputVertices)[0]), sizeof(Vertex) * outputVertices->size());
		file.close();
	}

	return true;
}

void Model::GetWorldMatrix(XMMATRIX& worldMatrix)
{
	worldMatrix = this->worldMatrix;
	return;
}

void Model::SetWorldMatrix(XMMATRIX worldMatrix)
{
	this->worldMatrix = worldMatrix;
	return;
}

int Model::NrOfSubsets()
{
	return this->subsetIndices.size();
}

void Model::GetSubsetInfo(int subsetIndex, int& indexStart, int& indexCount, int& textureIndex, XMFLOAT4& difColor, XMFLOAT4& specColor, bool& transparent)
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
	
	difColor = tempMaterial.difColor;
	specColor = tempMaterial.specColor;
	transparent = tempMaterial.transparent;

	return;
}