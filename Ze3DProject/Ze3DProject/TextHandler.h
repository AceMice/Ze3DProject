#ifndef MODEL_H
#define MODEL_H

#include "Texture.h"

class TextHandler {

public:
	struct Sentence {
		ID3D11Buffer* vertexBuffer;
		ID3D11Buffer* indexBuffer;
		int vertexCount;
		int indexCount;
	};

	struct Vertex {
		XMFLOAT3 position;
		XMFLOAT2 texture;

		Vertex()
		{
			position = XMFLOAT3(0.0f, 0.0f, 0.0f);
			texture = XMFLOAT2(0.0f, 0.0f);
		}
		Vertex(const Vertex& other)
		{
			position = other.position;
			texture = other.texture;
		}
		Vertex& operator=(const Vertex& other)
		{
			position = other.position;
			texture = other.texture;

			return *this;
		}
	};

private:
	XMMATRIX baseViewMatrix;
	Sentence* fpsCounter;
	int screenWidth;
	int screenHeight;
	int id;

	bool CreateSentence(ID3D11Device* device, int maxLength);
	void ReleaseSentence();
	void RenderSentence(ID3D11DeviceContext*);

public:
	TextHandler();
	TextHandler(const TextHandler&);
	~TextHandler();

	bool Initialize(ID3D11Device*, ID3D11DeviceContext*);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetId();
};



#endif