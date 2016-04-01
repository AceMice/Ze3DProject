#ifndef FONT_H
#define FONT_H

#include "Texture.h"


class Font
{
private:
	struct FontChar
	{
		float left, right;
		int size;
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

public:
	Font();
	Font(const Font&);
	~Font();

	bool Initialize(ID3D11Device*, char*, WCHAR*);
	void Shutdown();

	ID3D11ShaderResourceView* GetTexture();
	void BuildVertexArray(Vertex*, char*, float, float);

private:
	bool LoadFontData(std::string fontFilename);
	void ReleaseFontData();
	bool LoadTexture(ID3D11Device*, std::string textureFilename);
	void ReleaseTexture();

private:
	FontChar* fontChar;
	Texture* fontTexture;
};

#endif
