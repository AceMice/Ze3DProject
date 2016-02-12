#ifndef MODELHANDLER_H
#define MODELHANDLER_H

#include<d3d11.h>
#include<DirectXMath.h>
using namespace DirectX;

class ModelHandler {

private:
	
	struct VertexType {
		XMFLOAT3 position;
		XMFLOAT4 color;
	};

	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	int vertexCount;
	int indexCount;

public:
	ModelHandler();
	ModelHandler(const ModelHandler&);
	~ModelHandler();

	bool Initialize( ID3D11Device*);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();

};



#endif