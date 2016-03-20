#ifndef MODELWINDOW_H
#define MODELWINDOW_H

#include "Texture.h"

class ModelWindow {

private:

	struct VertexSimple {
		XMFLOAT3 position;
		XMFLOAT2 texture;

		VertexSimple()
		{
			position = XMFLOAT3(0.0f, 0.0f, 0.0f);
			texture = XMFLOAT2(0.0f, 0.0f);
		}
		VertexSimple(const VertexSimple& other)
		{
			position = other.position;
			texture = other.texture;
		}
		VertexSimple& operator=(const VertexSimple& other)
		{
			position = other.position;
			texture = other.texture;

			return *this;
		}
	};

	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	int vertexCount;
	int indexCount;
	XMMATRIX worldMatrix;

public:
	ModelWindow();
	ModelWindow(const ModelWindow&);
	~ModelWindow();

	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, int, int);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();
	void GetWorldMatrix(XMMATRIX&);
	void SetWorldMatrix(XMMATRIX);
};



#endif