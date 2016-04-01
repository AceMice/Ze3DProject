#include "Font.h"

Font::Font()
{
	this->fontChar = nullptr;
	this->fontTexture = nullptr;
}

Font::~Font()
{
}

bool Font::LoadFontData(std::string fontFilename)
{


	return true;
}

void Font::ReleaseFontData()
{

	return;
}

bool Font::LoadTexture(ID3D11Device*, std::string textureFilename)
{

	return true;
}

void Font::ReleaseTexture()
{

	return;
}

bool Font::Initialize(ID3D11Device*, char*, WCHAR*)
{


	return true;
}
void Font::Shutdown()
{
	
	return;
}

ID3D11ShaderResourceView* Font::GetTexture()
{
	return this->fontTexture->GetTexture(0);
}

void Font::BuildVertexArray(Vertex*, char*, float, float)
{


	return;
}