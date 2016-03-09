#ifndef Model_H
#define Model_H

#include "Texture.h"

class Model {

private:
	
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

	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	int vertexCount;
	int indexCount;
	std::vector<int> subsetIndices;
	std::vector<std::string> materialNames;
	Texture* texture;
	XMMATRIX worldMatrix;
	std::string name;

	bool InitializeBuffers(ID3D11Device*, char*, std::string&);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);
	bool LoadTexture(ID3D11Device*, ID3D11DeviceContext*, std::string);
	void ReleaseTexture();
	bool LoadObj(const char*, std::vector<Vertex>&, unsigned long*&, int&, int&, std::string&);

public:
	Model();
	Model(const Model&);
	~Model();

	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, char*);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();
	void GetWorldMatrix(XMMATRIX&);
	void SetWorldMatrix(XMMATRIX);

	ID3D11ShaderResourceView* GetTexture(int);
	int NrOfSubsets();
	void GetSubsetInfo(int, int&, int&, int&, XMFLOAT4&, XMFLOAT4&, bool&);

	std::string GetName();
};



#endif