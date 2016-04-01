#ifndef TEXTHANDLER_H
#define TEXTHANDLER_H

#include "Texture.h"
#include "Font.h"
#include "FontShaderHandler.h"

class TextHandler {

public:
	struct Sentence {
		ID3D11Buffer* vertexBuffer;
		ID3D11Buffer* indexBuffer;
		int vertexCount;
		int indexCount;
		int maxLength;
		XMFLOAT3 color;

		Sentence() 
		{
			this->vertexBuffer = nullptr;
			this->indexBuffer = nullptr;
		}

		void ShutdownBuffers()
		{
			//Release the index buffer
			if (this->indexBuffer) {
				this->indexBuffer->Release();
				this->indexBuffer = nullptr;
			}

			//Release the vertex buffer
			if (this->vertexBuffer) {
				this->vertexBuffer->Release();
				this->vertexBuffer = nullptr;
			}
		}
	};

	struct Vertex {
		XMFLOAT3 position;
		XMFLOAT2 texture;
	};

private:
	XMMATRIX baseViewMatrix;
	Font* font;
	std::vector<Sentence*> sentences;
	FontShaderHandler* fontShaderH;
	int screenWidth;
	int screenHeight;


	void RenderSentence(ID3D11DeviceContext* deviceContext, int id);
	void ReleaseSentences();

public:
	TextHandler();
	TextHandler(const TextHandler&);
	~TextHandler();

	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, XMMATRIX baseViewMatrix, int screenWidth, int screenHeight);
	void Shutdown();
	void Render(ID3D11DeviceContext* deviceContext, XMMATRIX orthoMatrix);
	int CreateSentence(ID3D11Device* device, int maxLength);
	bool UpdateSentence(ID3D11DeviceContext* deviceContext, int id, std::string text, int newPosX, int newPosY, XMFLOAT3 color);
};



#endif