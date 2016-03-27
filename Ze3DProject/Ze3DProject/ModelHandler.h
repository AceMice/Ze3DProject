#ifndef MODELHANDLER_H
#define MODELHANDLER_H


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

		void GetBoundBox(XMVECTOR* boundingBox)
		{
			boundingBox[0] = XMVectorSet(minBox.x, maxBox.y, maxBox.z, 1.0f);

			boundingBox[1] = XMVectorSet(minBox.x, maxBox.y, minBox.z, 1.0f);

			boundingBox[2] = XMVectorSet(maxBox.x, maxBox.y, minBox.z, 1.0f);

			boundingBox[3] = XMVectorSet(maxBox.x, maxBox.y, maxBox.z, 1.0f);

			boundingBox[4] = XMVectorSet(minBox.x, minBox.y, maxBox.z, 1.0f);

			boundingBox[5] = XMVectorSet(minBox.x, minBox.y, minBox.z, 1.0f);

			boundingBox[6] = XMVectorSet(maxBox.x, minBox.y, minBox.z, 1.0f);

			boundingBox[7] = XMVectorSet(maxBox.x, minBox.y, maxBox.z, 1.0f);

			return;
		}
	};
	std::vector<Model*> models;
	std::vector<Model*> modelsNoBB;
	QuadNode* quadTree;

	void DeleteQuadNode(QuadNode* node);
	void CreateQuadrants(QuadNode* node, int level);
	bool BBIntersect(XMFLOAT3 minBB1, XMFLOAT3 maxBB1, XMFLOAT3 minBB2, XMFLOAT3 maxBB2);
	
	void TraverseQuadTree(QuadNode* node, Frustum* viewFrustum, std::vector<QuadNode*>& nodesToRender);
	Model* GetModelFromName(std::string name);
public:
	ModelHandler();
	~ModelHandler();

	bool Initialize();
	bool CreateModel(ID3D11Device* device, ID3D11DeviceContext* deviceContext, std::string modelObj, std::string modelName, bool hasBB);
	bool UpdateModelWorldMatrix(std::string modelName, XMMATRIX worldMatrix);
	bool CreateQuadTree(int levels);
	std::vector<Model*> GetModelsInViewFrustum(Frustum* viewFrustum);
	std::vector<Model*> GetModels();
	void Shutdown();
};

#endif