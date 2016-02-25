#ifndef TEXTURE_H
#define TEXTURE_H

#include <d3d11.h>
#include <stdio.h>
#include <string>

class Texture {

private:
	unsigned char* targaData;
	ID3D11Texture2D* texture;
	ID3D11ShaderResourceView* textureView;
	
	struct TargaHeader
	{
		unsigned char data1[12];
		unsigned short width;
		unsigned short height;
		unsigned char bpp;
		unsigned char data2;
	};

	bool LoadTarga(const char*, int&, int&);


public:
	Texture();
	Texture(const Texture&);
	~Texture();

	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, char*, bool);
	void Shutdown();
	ID3D11ShaderResourceView* GetTexture();
};


#endif
