#ifndef MODELHANDLER_H
#define MODELHANDLER_H

#include<d3d11.h>
#include<DirectXMath.h>
using namespace DirectX;

#include "Texture.h"

class ModelHandler {

private:
	
	struct Vertex {
		XMFLOAT3 position;
		XMFLOAT2 texture;
	};

	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	int vertexCount;
	int indexCount;
	Texture* texture;

	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);
	bool LoadTexture(ID3D11Device*, ID3D11DeviceContext*, char*);
	void ReleaseTexture();

public:
	ModelHandler();
	ModelHandler(const ModelHandler&);
	~ModelHandler();

	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, char*);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();

	ID3D11ShaderResourceView* GetTexture();
};



#endif