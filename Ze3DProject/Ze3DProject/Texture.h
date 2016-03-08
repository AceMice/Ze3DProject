#ifndef TEXTURE_H
#define TEXTURE_H

#include <d3d11.h>
#include <stdio.h>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <d3d11.h>
#include <DirectXMath.h>

using namespace DirectX;

class Texture {
public:
	struct Material
	{
		std::string name;
		XMFLOAT4 difColor;
		XMFLOAT4 specColor;
		int textureIndex;
		bool hasTexture;
		bool transparent;
	};

private:
	
	struct TargaHeader
	{
		unsigned char data1[12];
		unsigned short width;
		unsigned short height;
		unsigned char bpp;
		unsigned char data2;
	};

	unsigned char* targaData;
	ID3D11Texture2D* texture;
	std::vector<ID3D11ShaderResourceView*> textureViews;
	std::vector<Material> materials;
	std::vector<std::string> textureNames;

	bool LoadTarga(const char*, int&, int&);
	bool LoadMtl(ID3D11Device*, ID3D11DeviceContext*, std::string);

public:
	Texture();
	Texture(const Texture&);
	~Texture();

	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, std::string);
	void Shutdown();
	ID3D11ShaderResourceView* GetTexture(int);
	Material GetMaterial(std::string);
};


#endif
