#include "Model.h"


Model::Model() 
{
	this->vertexBuffer = nullptr;
	this->indexBuffer = nullptr;
	this->texture = nullptr;
	this->worldMatrix = XMMatrixIdentity();
	this->isSelected = false;
}

Model::Model(const Model& originalObj)
{

}

Model::~Model() 
{

}

bool Model::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, std::string modelFilename, std::string modelName, int modelId, bool hasBB, std::vector<Vertex>* verticesIn)
{
	bool result;
	std::string materialLib;

	this->name = modelName;
	this->id = modelId;
	this->hasBB = hasBB;
	

	//Initialze the vertex and index buffer
	result = this->InitializeBuffers(device, modelFilename, materialLib, verticesIn);
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

bool Model::InitializeBuffers(ID3D11Device* device, std::string modelFilename, std::string& materialName, std::vector<Vertex>* verticesIn)
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
	if (!verticesIn) {
		std::string path = "../Ze3DProject/OBJ/";
		std::string finalPath = path + modelFilename;
		result = this->LoadObj(finalPath.c_str(), vertices, indices, sizeVertices, sizeIndices, materialName);
		if (!result) {
			return false;
		}
		//Set the number of vertices in the vertex array
		this->vertexCount = sizeVertices;
		//Set the numer of indices in the index array
		this->indexCount = sizeIndices;
	}
	else { //Make box model from incoming vertices for debugging quadtree
		for (int i = 0; i < verticesIn->size(); i++) {
			vertices.push_back(verticesIn->at(i));
		}
		this->vertexCount = vertices.size();
		this->indexCount = this->vertexCount;
		indices = new unsigned long[this->indexCount];
		for (int i = 0; i < this->indexCount; i++) {
			indices[i] = i;
		}
		materialName = "box.mtl";
		this->subsetIndices.push_back(0);
		this->materialNames.push_back("box");
	}

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
	vertices.clear();

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

bool Model::LoadObj(const char* filename, std::vector<Vertex>& outputVertices, unsigned long*& outputIndices, int& sizeVertices, int& sizeIndices, std::string& materialLib)
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
	double point[3];
	char junk;
	std::stringstream ss;
	std::fstream file;
	bool newGroup = false;
	int tempSubset = 0;
	std::string format = ".bin";
	std::string path = filename;
	path.append(".ace");
	file.open(path, std::ios::in);
	if (file.is_open()) { //Model has been loaded before and therefor has binary files to read
		std::getline(file, line);
		ss.str(line);
		ss >> sizeVertices >> sizeIndices; //Read the sizes

		std::getline(file, line);
		ss.clear();
		ss.str(line);
		ss >> materialLib; //Read the .mtl filename

		std::getline(file, line);
		ss.clear();
		ss.str(line);
		ss >> this->hasBB;
		if (this->hasBB) { //If it has a bounding box, read the min max
			std::getline(file, line);
			ss.clear();
			ss.str(line);
			ss >> this->minVertex.x >> this->minVertex.y >> this->minVertex.z;
			std::getline(file, line);
			ss.clear();
			ss.str(line);
			ss >> this->maxVertex.x >> this->maxVertex.y >> this->maxVertex.z;
		}

		while (std::getline(file, line)) { //Read all the subset indices and corresponding materials
			ss.clear();
			ss.str(line);
			ss >> tempSubset >> tempLine;
			subsetIndices.push_back(tempSubset);
			materialNames.push_back(tempLine);
		}
		ss.clear();
		file.close();
		path = filename;
		path.append("V.bin");
		file.open(path, std::ios::binary | std::ios::in);
		if (!file.is_open()) {
			return false;
		}
		Vertex* tempVerticesArray = new Vertex[sizeVertices];

		file.read((char*)tempVerticesArray, sizeof(Vertex) * sizeVertices); //Read the binary file containing the vertex data and save for output
		file.close();

		outputVertices.insert(outputVertices.end(), &tempVerticesArray[0], &tempVerticesArray[sizeVertices]);
		this->vertPositions.insert(this->vertPositions.end(), &tempVerticesArray[0].position, &tempVerticesArray[sizeVertices].position);

		delete[] tempVerticesArray;

		path = filename;
		path.append("I.bin");
		file.open(path, std::ios::binary | std::ios::in);
		if (!file.is_open()) {
			return false;
		}
		
		outputIndices = new unsigned long[sizeIndices];
		file.read((char*)outputIndices, sizeof(unsigned long) * sizeVertices);//Read the binary file containing the index data and save for output
		file.close();
	}
	else { //If it's the first time models is being loaded read and parse the obj file
		format = ".obj";
		path = filename + format;
		file.open(path, std::ios::in);
		if (!file.is_open()) {
			return false;
		}
		XMFLOAT3 maxVert = XMFLOAT3(-D3D11_FLOAT32_MAX, -D3D11_FLOAT32_MAX, -D3D11_FLOAT32_MAX);
		XMFLOAT3 minVert = XMFLOAT3(D3D11_FLOAT32_MAX, D3D11_FLOAT32_MAX, D3D11_FLOAT32_MAX);
		while (std::getline(file, line)) {
			if (line.size() > 0) {
				if (line.at(0) == 'v') {
					if (line.at(1) == ' ') { //Save vertex position coordinates
						ss.clear();
						ss.str(line);
						ss >> junk >> tempVertex.x >> tempVertex.y >> tempVertex.z;

						if (tempVertex.x > maxVert.x) {
							maxVert.x = tempVertex.x;
						}
						if (tempVertex.y > maxVert.y) {
							maxVert.y = tempVertex.y;
						}
						if (tempVertex.z > maxVert.z) {
							maxVert.z = tempVertex.z;
						}
						if (tempVertex.x < minVert.x) {
							minVert.x = tempVertex.x;
						}
						if (tempVertex.y < minVert.y) {
							minVert.y = tempVertex.y;
						}
						if (tempVertex.z < minVert.z) {
							minVert.z = tempVertex.z;
						}
						
						tempVertices.push_back(tempVertex);
					}
					else if (line.at(1) == 't') { //Save vertex texture coordiantes
						ss.clear();
						ss.str(line);
						ss >> junks >> tempUV.x >> tempUV.y;
						tempUV.y = 1.0f - tempUV.y;
						tempUvs.push_back(tempUV);
					}
					else if (line.at(1) == 'n') { //Save the vertex normals
						ss.clear();
						ss.str(line);
						ss >> junks >> tempNormal.x >> tempNormal.y >> tempNormal.z;
						tempNormals.push_back(tempNormal);
					}
				}
				else if (line.at(0) == 'g') { //Create a new subset
					this->subsetIndices.push_back(vertexIndices.size());
					newGroup = true;
				}
				else if (line.substr(0, 6) == "usemtl") { //Subset uses this material
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
				else if (line.at(0) == 'f') { //Save the vertices order from faces
					if (newGroup) {
						this->materialNames.push_back(this->materialNames.back());
						newGroup = false;
					}
					ss.clear();
					ss.str(line);
					ss >> junk >> vertexIndex[0] >> junk >> uvIndex[0] >> junk >> normalIndex[0]
						>> vertexIndex[1] >> junk >> uvIndex[1] >> junk >> normalIndex[1]
						>> vertexIndex[2] >> junk >> uvIndex[2] >> junk >> normalIndex[2];

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
				else if (line.substr(0, 6) == "mtllib") { //Save the .mtl filename
					ss.clear();
					ss.str(line);
					ss >> junks >> materialLib;
				}
			}
		}
		file.close();

		if (vertexIndices.size() == 0 || tempVertices.size() == 0) { //Makr sure the obj wasn't empty
			return false;
		}

		sizeVertices = vertexIndices.size();
		sizeIndices = vertexIndices.size();
		outputIndices = new unsigned long[sizeIndices];
		this->vertPositions = tempVertices;

		for (int i = 0; i < sizeVertices; i++) { //Create the output vertex array
			Vertex tempVertex;
			tempVertex.position = tempVertices.at(vertexIndices.at(i) - 1);
			tempVertex.texture = tempUvs.at(uvIndices.at(i) - 1);
			tempVertex.normal = tempNormals.at(normalIndices.at(i) - 1);
			outputVertices.push_back(tempVertex);

			outputIndices[i] = i;
		}

		this->minVertex = minVert;
		this->maxVertex = maxVert;
		

		path = filename;
		path.append(".ace"); //Make the "All Computations Executed" file
		file.open(path, std::ios::out);
		if (!file.is_open()) {
			return false;
		}
		file << sizeVertices << " " << sizeIndices << "\n"; //Save the sizes, needed for loading the binary files
		file << materialLib << "\n";
		
		if (this->hasBB) {
			file << 1 << "\n";
			file << this->minVertex.x << " " << this->minVertex.y << " " << this->minVertex.z << "\n";
			file << this->maxVertex.x << " " << this->maxVertex.y << " " << this->maxVertex.z << "\n";
		}
		else {
			file << 0  << "\n";
		}
		
		for (int i = 0; i < subsetIndices.size(); i++) {
			file << subsetIndices.at(i) << " " << materialNames.at(i) << "\n"; //Save all subsets with each material
		}
		file.close();

		format = "V.bin"; //Save the vericies as binary
		path = filename + format;
		file.open(path, std::ios::out | std::ios::binary);
		if (!file.is_open()) {
			return false;
		}
		file.write((char*)&(outputVertices[0]), sizeof(Vertex) * outputVertices.size());
		file.close();

		format = "I.bin"; //save the indices as binary
		path = filename + format;
		file.open(path, std::ios::out | std::ios::binary);
		if (!file.is_open()) {
			return false;
		}
		file.write((char*)outputIndices, sizeof(unsigned long) * sizeIndices);
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

void Model::GetSubsetInfo(int subsetIndex, int& indexStart, int& indexCount, int& textureIndex, int& normMapIndex, XMFLOAT4& difColor, XMFLOAT4& specColor, bool& transparent, bool& picked)
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

	picked = this->isSelected;

	return;
}

std::string Model::GetName()
{
	return this->name;
}


XMVECTOR* Model::GetBouningBox(XMMATRIX MVP)
{
	XMVECTOR* worldBB = new XMVECTOR[8];
	for (int i = 0; i < 8; i++) {
		worldBB[i] = XMVector4Transform(worldBB[i], MVP);
	}
	return worldBB;
}

bool Model::GethasBB()
{
	return this->hasBB;
}

int Model::GetId()
{
	return this->id;
}

void Model::GetMinMaxVertex(XMFLOAT3& minVert, XMFLOAT3& maxVert)
{
	minVert = this->minVertex;
	maxVert = this->maxVertex;

	return;
}

void Model::GenerateMinMaxVertex()
{
	XMVECTOR vertVec;
	XMFLOAT3 vertFloat;
	XMFLOAT3 maxVert = XMFLOAT3(-D3D11_FLOAT32_MAX, -D3D11_FLOAT32_MAX, -D3D11_FLOAT32_MAX);
	XMFLOAT3 minVert = XMFLOAT3(D3D11_FLOAT32_MAX, D3D11_FLOAT32_MAX, D3D11_FLOAT32_MAX);

	for (int i = 0; i < this->vertPositions.size(); i++) {
		vertVec = XMLoadFloat3(&this->vertPositions.at(i));
		vertVec = XMVector3TransformCoord(vertVec, this->worldMatrix);
		XMStoreFloat3(&vertFloat, vertVec);

		if (vertFloat.x > maxVert.x) {
			maxVert.x = vertFloat.x;
		}
		if (vertFloat.y > maxVert.y) {
			maxVert.y = vertFloat.y;
		}
		if (vertFloat.z > maxVert.z) {
			maxVert.z = vertFloat.z;
		}
		if (vertFloat.x < minVert.x) {
			minVert.x = vertFloat.x;
		}
		if (vertFloat.y < minVert.y) {
			minVert.y = vertFloat.y;
		}
		if (vertFloat.z < minVert.z) {
			minVert.z = vertFloat.z;
		}
	}

	this->minVertex = minVert;
	this->maxVertex = maxVert;
}

bool Model::IsModelSelected() {
	return this->isSelected;
}

void Model::SetModelSelectionState(bool newState) {
	this->isSelected = newState;
}