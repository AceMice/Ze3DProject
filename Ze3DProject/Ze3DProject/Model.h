#ifndef Model_H
#define Model_H

#include<d3d11.h>
#include<DirectXMath.h>
#include<fstream>
#include<string>
#include<sstream>
#include<vector>
using namespace DirectX;

#include "Texture.h"

class Model {

private:
	
	struct Vertex {
		XMFLOAT3 position;
		XMFLOAT2 texture;
		//XMFLOAT3 normal;
	};

	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	int vertexCount;
	int indexCount;
	Texture* texture;

	bool InitializeBuffers(ID3D11Device*, char*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);
	bool LoadTexture(ID3D11Device*, ID3D11DeviceContext*, char*);
	void ReleaseTexture();
	bool LoadObj(const char*, Vertex*&, unsigned long*&, int&, int&);

public:
	Model();
	Model(const Model&);
	~Model();

	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, char*);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();

	ID3D11ShaderResourceView* GetTexture();
};



#endif