#include "ModelHandler.h"

ModelHandler::ModelHandler()
{
	this->quadTree = nullptr;
	this->pickedModels = 0;
}

ModelHandler::~ModelHandler()
{

}

void ModelHandler::QuadNode::GetBoundBox(XMVECTOR* boundingBox)
{
	boundingBox[0] = XMVectorSet(this->minBox.x, this->maxBox.y, this->maxBox.z, 1.0f);

	boundingBox[1] = XMVectorSet(this->minBox.x, this->maxBox.y, this->minBox.z, 1.0f);

	boundingBox[2] = XMVectorSet(this->maxBox.x, this->maxBox.y, this->minBox.z, 1.0f);

	boundingBox[3] = XMVectorSet(this->maxBox.x, this->maxBox.y, this->maxBox.z, 1.0f);

	boundingBox[4] = XMVectorSet(this->minBox.x, this->minBox.y, this->maxBox.z, 1.0f);

	boundingBox[5] = XMVectorSet(this->minBox.x, this->minBox.y, this->minBox.z, 1.0f);

	boundingBox[6] = XMVectorSet(this->maxBox.x, this->minBox.y, this->minBox.z, 1.0f);

	boundingBox[7] = XMVectorSet(this->maxBox.x, this->minBox.y, this->maxBox.z, 1.0f);

	return;
	
}

void ModelHandler::QuadNode::GetVertices(std::vector<Model::Vertex>* vertices)
{
	XMVECTOR* boudingBox = new XMVECTOR[8];
	this->GetBoundBox(boudingBox);
	Model::Vertex tempVert;

	XMStoreFloat3(&tempVert.position, boudingBox[0]); //Top quad
	vertices->push_back(tempVert);
	XMStoreFloat3(&tempVert.position, boudingBox[2]);
	vertices->push_back(tempVert);
	XMStoreFloat3(&tempVert.position, boudingBox[1]);
	vertices->push_back(tempVert);
	XMStoreFloat3(&tempVert.position, boudingBox[0]);
	vertices->push_back(tempVert);
	XMStoreFloat3(&tempVert.position, boudingBox[3]);
	vertices->push_back(tempVert);
	XMStoreFloat3(&tempVert.position, boudingBox[2]);
	vertices->push_back(tempVert);

	XMStoreFloat3(&tempVert.position, boudingBox[4]); //Front quad
	vertices->push_back(tempVert);
	XMStoreFloat3(&tempVert.position, boudingBox[3]);
	vertices->push_back(tempVert);
	XMStoreFloat3(&tempVert.position, boudingBox[0]);
	vertices->push_back(tempVert);
	XMStoreFloat3(&tempVert.position, boudingBox[4]);
	vertices->push_back(tempVert);
	XMStoreFloat3(&tempVert.position, boudingBox[7]);
	vertices->push_back(tempVert);
	XMStoreFloat3(&tempVert.position, boudingBox[3]);
	vertices->push_back(tempVert);

	XMStoreFloat3(&tempVert.position, boudingBox[5]); //Left quad
	vertices->push_back(tempVert);
	XMStoreFloat3(&tempVert.position, boudingBox[0]);
	vertices->push_back(tempVert);
	XMStoreFloat3(&tempVert.position, boudingBox[1]);
	vertices->push_back(tempVert);
	XMStoreFloat3(&tempVert.position, boudingBox[5]);
	vertices->push_back(tempVert);
	XMStoreFloat3(&tempVert.position, boudingBox[4]);
	vertices->push_back(tempVert);
	XMStoreFloat3(&tempVert.position, boudingBox[0]);
	vertices->push_back(tempVert);

	XMStoreFloat3(&tempVert.position, boudingBox[6]); //Back quad
	vertices->push_back(tempVert);
	XMStoreFloat3(&tempVert.position, boudingBox[1]);
	vertices->push_back(tempVert);
	XMStoreFloat3(&tempVert.position, boudingBox[2]);
	vertices->push_back(tempVert);
	XMStoreFloat3(&tempVert.position, boudingBox[6]);
	vertices->push_back(tempVert);
	XMStoreFloat3(&tempVert.position, boudingBox[5]);
	vertices->push_back(tempVert);
	XMStoreFloat3(&tempVert.position, boudingBox[1]);
	vertices->push_back(tempVert);

	XMStoreFloat3(&tempVert.position, boudingBox[7]); //Right quad
	vertices->push_back(tempVert);
	XMStoreFloat3(&tempVert.position, boudingBox[2]);
	vertices->push_back(tempVert);
	XMStoreFloat3(&tempVert.position, boudingBox[3]);
	vertices->push_back(tempVert);
	XMStoreFloat3(&tempVert.position, boudingBox[7]);
	vertices->push_back(tempVert);
	XMStoreFloat3(&tempVert.position, boudingBox[6]);
	vertices->push_back(tempVert);
	XMStoreFloat3(&tempVert.position, boudingBox[2]);
	vertices->push_back(tempVert);

	XMStoreFloat3(&tempVert.position, boudingBox[5]); //Bottom quad
	vertices->push_back(tempVert);
	XMStoreFloat3(&tempVert.position, boudingBox[7]);
	vertices->push_back(tempVert);
	XMStoreFloat3(&tempVert.position, boudingBox[4]);
	vertices->push_back(tempVert);
	XMStoreFloat3(&tempVert.position, boudingBox[5]);
	vertices->push_back(tempVert);
	XMStoreFloat3(&tempVert.position, boudingBox[6]);
	vertices->push_back(tempVert);
	XMStoreFloat3(&tempVert.position, boudingBox[7]);
	vertices->push_back(tempVert);

	delete[] boudingBox;
}

bool ModelHandler::Initialize()
{
	

	return true;
}

bool ModelHandler::CreateModel(ID3D11Device* device, ID3D11DeviceContext* deviceContext, std::string modelObj, std::string modelName, bool hasBB)
{
	bool result;
	Model* tempModel = nullptr;

	tempModel = new Model;
	if (!tempModel)
	{
		return false;
	}
	//Initialize the OgreFullG object
	result = tempModel->Initialize(device, deviceContext, modelObj, modelName, this->models.size(), hasBB);
	if (!result)
	{
		return false;
	}
	//Insert model into vector
	if (hasBB) {
		this->models.push_back(tempModel);
	}
	else {
		this->modelsNoBB.push_back(tempModel);
	}
	

	return true;
}

void ModelHandler::DeleteQuadNode(QuadNode* node)
{
	if (node == nullptr) {
		return;
	}

	for (int i = 0; i < 4; i++) {
		if (node->child[i]) {
			this->DeleteQuadNode(node->child[i]);
			
		}
	}

	delete node;
	node = nullptr;

	return;
}

void ModelHandler::Shutdown()
{
	if (this->quadTree) {
		this->DeleteQuadNode(this->quadTree);
	}

	for (int i = 0; i < this->models.size(); i++) {
		if (this->models.at(i)) {
			this->models.at(i)->Shutdown();
			delete this->models.at(i);
			this->models.at(i) = nullptr;
		}
	}
	for (int i = 0; i < this->modelsNoBB.size(); i++) {
		if (this->modelsNoBB.at(i)) {
			this->modelsNoBB.at(i)->Shutdown();
			delete this->modelsNoBB.at(i);
			this->modelsNoBB.at(i) = nullptr;
		}
	}
}

bool ModelHandler::BBIntersect(XMFLOAT3 minBB1, XMFLOAT3 maxBB1, XMFLOAT3 minBB2, XMFLOAT3 maxBB2)
{
	return (minBB1.x <= maxBB2.x && maxBB1.x >= minBB2.x) &&
		(minBB1.y <= maxBB2.y && maxBB1.y >= minBB2.y) &&
		(minBB1.z <= maxBB2.z && maxBB1.z >= minBB2.z);
}

bool ModelHandler::CreateBBModel(ID3D11Device* device, ID3D11DeviceContext* deviceContext, QuadNode* node)
{
	if (node == nullptr) {
		return false;
	}

	bool result = false;

	std::vector<Model::Vertex>* vertices = new std::vector<Model::Vertex>;
	node->GetVertices(vertices);
	Model* tempModel = nullptr;
	tempModel = new Model;
	if (!tempModel)
	{
		return false;
	}
	//Initialize the bounding box model object
	result = tempModel->Initialize(device, deviceContext, "", "ModelBB", 0, false, vertices);
	if (!result)
	{
		return false;
	}

	delete vertices;

	this->modelsNoBB.push_back(tempModel);

	return true;
}

bool ModelHandler::CreateBBModels(ID3D11Device* device, ID3D11DeviceContext* deviceContext, QuadNode* node)
{
	if (node == nullptr) {
		return true;
	}

	bool result = false;

	for (int i = 0; i < 4; i++) {
		result = this->CreateBBModels(device, deviceContext, node->child[i]);
		if (!result) {
			return false;
		}
	}
	
	std::vector<Model::Vertex>* vertices = new std::vector<Model::Vertex>;
	node->GetVertices(vertices);
	Model* tempModel = nullptr;
	tempModel = new Model;
	if (!tempModel)
	{
		return false;
	}
	//Initialize the bounding box model object
	result = tempModel->Initialize(device, deviceContext, "", "ModelBB", 0, false, vertices);
	if (!result)
	{
		return false;
	}

	this->modelsNoBB.push_back(tempModel);

	delete vertices;

	return true;
}

void ModelHandler::CreateQuadrants(QuadNode* node, int level)
{
	if (level == 0) {
		return;
	}
	float dx = (node->maxBox.x - node->minBox.x) * 0.5f;
	float dy = (node->maxBox.y - node->minBox.y);
	float dz = (node->maxBox.z - node->minBox.z) * 0.5f;

	for (int i = 0; i < 4; i++) {
		node->child[i] = new QuadNode;
	}

	node->child[0]->minBox = node->minBox;
	node->child[0]->maxBox = XMFLOAT3(node->maxBox.x - dx, node->maxBox.y, node->maxBox.z - dz);

	node->child[1]->minBox = XMFLOAT3(node->minBox.x + dx, node->minBox.y , node->minBox.z);
	node->child[1]->maxBox = XMFLOAT3(node->maxBox.x, node->maxBox.y, node->maxBox.z - dz);

	node->child[2]->minBox = XMFLOAT3(node->minBox.x, node->minBox.y, node->minBox.z + dz);
	node->child[2]->maxBox = XMFLOAT3(node->maxBox.x - dx, node->maxBox.y, node->maxBox.z);

	node->child[3]->minBox = XMFLOAT3(node->minBox.x + dx, node->minBox.y, node->minBox.z + dz);
	node->child[3]->maxBox = node->maxBox;

	XMFLOAT3 minModel, maxModel;
	for (int i = 0; i < node->models.size(); i++) {
		node->models.at(i)->GetMinMaxVertex(minModel, maxModel);
		for (int j = 0; j < 4; j++) {
			if (this->BBIntersect(node->child[j]->minBox, node->child[j]->maxBox, minModel, maxModel)) {
				node->child[j]->models.push_back(node->models.at(i));
			}
		}
	}
	
	for (int i = 0; i < 4; i++) {
		this->CreateQuadrants(node->child[i], level - 1);
	}
}

bool ModelHandler::CreateQuadTree(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int levels)
{
	XMFLOAT3 minModel, maxModel; 
	XMFLOAT3 minestModel = XMFLOAT3(D3D11_FLOAT32_MAX, D3D11_FLOAT32_MAX, D3D11_FLOAT32_MAX);
	XMFLOAT3 maxestModel = XMFLOAT3(-D3D11_FLOAT32_MAX, -D3D11_FLOAT32_MAX, -D3D11_FLOAT32_MAX);
	this->quadTree = new QuadNode;
	bool result = false;

	for (int i = 0; i < this->models.size(); i++) {
		this->quadTree->models.push_back(this->models.at(i));
		this->models.at(i)->GetMinMaxVertex(minModel, maxModel);
		if (maxModel.x > maxestModel.x) {
			maxestModel.x = maxModel.x;
		}
		if (maxModel.y > maxestModel.y) {
			maxestModel.y = maxModel.y;
		}
		if (maxModel.z > maxestModel.z) {
			maxestModel.z = maxModel.z;
		}
		if (minModel.x < minestModel.x) {
			minestModel.x = minModel.x;
		}
		if (minModel.y < minestModel.y) {
			minestModel.y = minModel.y;
		}
		if (minModel.z < minestModel.z) {
			minestModel.z = minModel.z;
		}
	}
	//this->quadTree->minBox = XMFLOAT3(-200, -10, -200);
	//this->quadTree->maxBox = XMFLOAT3(400, 40, 400);
	this->quadTree->minBox = minestModel;
	this->quadTree->maxBox = maxestModel;

	this->CreateQuadrants(this->quadTree, levels);

	/*result = this->CreateBBModels(device, deviceContext, this->quadTree);
	if (!result) {
		return false;
	}*/

	return true;
}

void ModelHandler::TraverseQuadTree(QuadNode* node, Frustum* viewFrustum, std::vector<QuadNode*>& nodesToRender)
{
	if (node->child[0] == nullptr) {
		nodesToRender.push_back(node);
		return;
	}
	
	XMVECTOR* boundingBox = new XMVECTOR[8];

	node->child[0]->GetBoundBox(boundingBox);
	if (viewFrustum->IntersectBB(boundingBox)) {
		this->TraverseQuadTree(node->child[0], viewFrustum, nodesToRender);
	}
	node->child[1]->GetBoundBox(boundingBox);
	if (viewFrustum->IntersectBB(boundingBox)) {
		this->TraverseQuadTree(node->child[1], viewFrustum, nodesToRender);
	}
	node->child[2]->GetBoundBox(boundingBox);
	if (viewFrustum->IntersectBB(boundingBox)) {
		this->TraverseQuadTree(node->child[2], viewFrustum, nodesToRender);
	}
	node->child[3]->GetBoundBox(boundingBox);
	if (viewFrustum->IntersectBB(boundingBox)) {
		this->TraverseQuadTree(node->child[3], viewFrustum, nodesToRender);
	}

	delete[] boundingBox;

	return;
}

std::vector<Model*> ModelHandler::GetModelsInViewFrustum(Frustum* viewFrustum)
{
	std::vector<QuadNode*> nodesToRender;
	std::vector<Model*> outputModels;
	std::vector<int> modelIDs;
	int modelID = 0;
	bool alreadyExists = false;

	this->TraverseQuadTree(this->quadTree, viewFrustum, nodesToRender);

	if (nodesToRender.size() != 21) {
		int lol = 0;
	}

	for (int i = 0; i < nodesToRender.size(); i++) {
		for (int j = 0; j < nodesToRender.at(i)->models.size(); j++) {
			alreadyExists = false;
			modelID = nodesToRender.at(i)->models.at(j)->GetId();
			for (int k = 0; k < modelIDs.size(); k++) {
				if (modelIDs.at(k) == modelID) {
					alreadyExists = true;
				}
			}

			if (!alreadyExists) {
				outputModels.push_back(nodesToRender.at(i)->models.at(j));
				modelIDs.push_back(modelID);
			}
		}
	}
	for (int i = 0; i < this->modelsNoBB.size(); i++) {
		outputModels.push_back(this->modelsNoBB.at(i));
	}

	return outputModels;
}

Model* ModelHandler::GetModelFromName(std::string name)
{
	//Find the correct model by name
	for (int i = 0; i < this->models.size(); i++) {
		if (this->models.at(i)->GetName() == name) {
			return this->models.at(i);
		}
	}
	for (int i = 0; i < this->modelsNoBB.size(); i++) {
		if (this->modelsNoBB.at(i)->GetName() == name) {
			return this->modelsNoBB.at(i);
		}
	}

	return NULL;
}

bool ModelHandler::UpdateModelWorldMatrix(std::string modelName, XMMATRIX worldMatrix)
{
	Model* tempModel = this->GetModelFromName(modelName);
	if (tempModel) {
		tempModel->SetWorldMatrix(worldMatrix);
		return true;
	}

	return false;
}

std::vector<Model*> ModelHandler::GetModels()
{
	std::vector<Model*> outputModels;

	for (int i = 0; i < this->models.size(); i++) {
		outputModels.push_back(this->models.at(i));
	}
	for (int i = 0; i < this->modelsNoBB.size(); i++) {
		outputModels.push_back(this->modelsNoBB.at(i));
	}

	return outputModels;
}

std::vector<Model*> ModelHandler::GetModelsInNode(int path[], int levels)
{
	std::vector<Model*> outputModels;
	QuadNode* node = this->quadTree;

	for (int i = 0; i < levels; i++) {
		if (path[i] == 0 || !node->child[path[i] - 1]) {
			for (int i = 0; i < node->models.size(); i++) {
				outputModels.push_back(node->models.at(i));
			}
			for (int i = 0; i < this->modelsNoBB.size(); i++) {
				outputModels.push_back(this->modelsNoBB.at(i));
			}

			return outputModels;
		}

		node = node->child[path[i] - 1];
	}

	return outputModels;
}

void ModelHandler::GenerateModelsMinMaxVerts()
{
	for (int i = 0; i < this->models.size(); i++) {
		this->models.at(i)->GenerateMinMaxVertex();
	}
}

bool ModelHandler::SelectModel(XMVECTOR mouseViewPos, CameraHandler* cameraH) {
	bool result = true;
	
	//Get our point in View space
	XMFLOAT3 min;
	XMFLOAT3 max;
	XMVECTOR pos = XMVectorSet(0,0,0,0);
	float t = NULL;
	float lastT = 99099;
	int index = -1;
	XMMATRIX CameraViewMatrix;
	cameraH->GetViewMatrix(CameraViewMatrix);
	XMMATRIX inverseViewMatrix = XMMatrixInverse(NULL,CameraViewMatrix);	//Invert the view matrix to move from view to worl space

	XMVECTOR dir = XMVector3TransformNormal(mouseViewPos,inverseViewMatrix); 
	XMVECTOR oriPos = XMVector3TransformCoord(pos, inverseViewMatrix);

	
	for (int i = 0; i < this->models.size(); i++) {
		this->models.at(i)->GetMinMaxVertex(min, max);

		if (this->RayAABBCheack(min, max, oriPos, dir, t)) {
			if (t < lastT) {
				index = i;
				lastT = t;
			}
		}
	}

	if (index != -1 && !this->models.at(index)->IsModelSelected()) {
		this->models.at(index)->SetModelSelectionState(true);
		this->pickedModels++;
	}
	else if (index != -1) {
		result = true;
	}
	else{
		result = false;
	}
	
	
	return result;
}

bool ModelHandler::RayAABBCheack(XMFLOAT3& min, XMFLOAT3& max, XMVECTOR ori, XMVECTOR dir, float& t) {
	XMFLOAT3 dirfrac;
	XMFLOAT3 dirf;
	XMFLOAT3 orif;
	XMStoreFloat3(&dirf,dir);
	XMStoreFloat3(&orif, ori);
	// r.dir is unit direction vector of ray
	dirfrac.x = 1.0f / dirf.x;
	dirfrac.y = 1.0f / dirf.y;
	dirfrac.z = 1.0f / dirf.z;
	// lb is the corner of AABB with minimal coordinates - left bottom, rt is maximal corner
	// r.org is origin of ray
	float t1 = (min.x - orif.x)*dirfrac.x;
	float t2 = (max.x - orif.x)*dirfrac.x;
	float t3 = (min.y - orif.y)*dirfrac.y;
	float t4 = (max.y - orif.y)*dirfrac.y;
	float t5 = (min.z - orif.z)*dirfrac.z;
	float t6 = (max.z - orif.z)*dirfrac.z;

	float tmin = max(max(min(t1, t2), min(t3, t4)), min(t5, t6));
	float tmax = min(min(max(t1, t2), max(t3, t4)), max(t5, t6));

	// if tmax < 0, ray (line) is intersecting AABB, but whole AABB is behing us
	if (tmax < 0)
	{
		t = tmax;
		return false;
	}

	// if tmin > tmax, ray doesn't intersect AABB
	if (tmin > tmax)
	{
		t = tmax;
		return false;
	}

	t = tmin;
	return true;
}

void ModelHandler::swap(float& v1, float& v2) {
	float temp = v1;
	v1 = v2;
	v2 = temp;
}

int ModelHandler::GetNrPickableModels()
{
	return this->models.size() - this->pickedModels;
}

void ModelHandler::resetSelectedModels()
{
	for (int i = 0; i < this->models.size(); i++) {
		this->models.at(i)->SetModelSelectionState(false);
		this->pickedModels = 0;
	}
}