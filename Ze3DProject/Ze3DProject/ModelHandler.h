#ifndef MODELHANDLER_H
#define MODELHANDLER_H

#include "CameraHandler.h"
#include "Model.h"
#include "Frustum.h"


class ModelHandler {
private:
	struct QuadNode {
		XMFLOAT3 minBox;
		XMFLOAT3 maxBox;
		std::vector<Model*> models;
		QuadNode* child[4];

		QuadNode()
		{
			for (int i = 0; i < 4; i++) {
				this->child[i] = nullptr;
			}
		}
		~QuadNode()
		{
			
		}

		void GetBoundBox(XMVECTOR* boundingBox);
		void GetVertices(std::vector<Model::Vertex>* vertices);
	};
	std::vector<Model*> models;
	std::vector<Model*> modelsNoBB;
	QuadNode* quadTree;
	int pickedModels;

	void DeleteQuadNode(QuadNode* node);
	void CreateQuadrants(QuadNode* node, int level);
	bool BBIntersect(XMFLOAT3 minBB1, XMFLOAT3 maxBB1, XMFLOAT3 minBB2, XMFLOAT3 maxBB2);
	
	void TraverseQuadTree(QuadNode* node, Frustum* viewFrustum, std::vector<QuadNode*>& nodesToRender);
	Model* GetModelFromName(std::string name);
	bool CreateBBModels(ID3D11Device* device, ID3D11DeviceContext* deviceContext, QuadNode* node);

	bool RayAABBCheack(XMFLOAT3&, XMFLOAT3&, XMVECTOR, XMVECTOR, float&);

public:
	ModelHandler();
	~ModelHandler();

	bool CreateModel(ID3D11Device* device, ID3D11DeviceContext* deviceContext, std::string modelObj, std::string modelName, bool hasBB);
	bool UpdateModelWorldMatrix(std::string modelName, XMMATRIX worldMatrix);
	bool CreateQuadTree(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int levels);
	std::vector<Model*> GetModelsInViewFrustum(Frustum* viewFrustum);
	std::vector<Model*> GetModels();
	std::vector<Model*> GetModelsInNode(int path[], int levels);
	void Shutdown();
	void GenerateModelsMinMaxVerts();
	bool SelectModel(XMVECTOR, CameraHandler*);
	void resetSelectedModels();
	
	int GetNrPickableModels();

};

#endif