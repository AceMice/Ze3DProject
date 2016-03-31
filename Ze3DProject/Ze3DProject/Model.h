#ifndef MODEL_H
#define MODEL_H

#include "Texture.h"

class Model {

public:
	
	struct Vertex {
		XMFLOAT3 position;
		XMFLOAT2 texture;
		XMFLOAT3 normal;

		Vertex()
		{
			position = XMFLOAT3(0.0f, 0.0f, 0.0f);
			texture = XMFLOAT2(0.0f, 0.0f);
			normal = XMFLOAT3(0.0f, 0.0f, 0.0f);
		}
		Vertex(const Vertex& other) 
		{
			position = other.position;
			texture = other.texture;
			normal = other.normal;
		}
		Vertex& operator=(const Vertex& other)
		{
			position = other.position;
			texture = other.texture;
			normal = other.normal;

			return *this;
		}
	};

private:

	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	int vertexCount;
	int indexCount;
	std::vector<int> subsetIndices;
	std::vector<std::string> materialNames;
	Texture* texture;
	XMMATRIX worldMatrix;
	std::string name;
	int id;
	XMVECTOR* boundingBox;
	XMFLOAT3 minVertex;
	XMFLOAT3 maxVertex;
	bool hasBB;
	std::vector<XMFLOAT3> vertPositions;
	
	bool InitializeBuffers(ID3D11Device*, std::string, std::string&, std::vector<Vertex>* vertices);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);
	bool LoadTexture(ID3D11Device*, ID3D11DeviceContext*, std::string);
	void ReleaseTexture();
	bool LoadObj(const char*, std::vector<Vertex>&, unsigned long*&, int&, int&, std::string&);
	void CreateBoundingBox(XMFLOAT3, XMFLOAT3);

public:
	Model();
	Model(const Model&);
	~Model();

	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, std::string, std::string, int, bool, std::vector<Vertex>* vertices = nullptr);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();
	void GetWorldMatrix(XMMATRIX&);
	void SetWorldMatrix(XMMATRIX);

	ID3D11ShaderResourceView* GetTexture(int);
	int NrOfSubsets();
	void GetSubsetInfo(int, int&, int&, int&, int&, XMFLOAT4&, XMFLOAT4&, bool&);

	std::string GetName();
	XMVECTOR* GetBouningBox(XMMATRIX);
	bool GethasBB();
	int GetId();
	void GetMinMaxVertex(XMFLOAT3& minVert, XMFLOAT3& maxVert);
	void GenerateMinMaxVertex();
};



#endif