#include "Texture.h"


Texture::Texture() 
{
	this->targaData = nullptr;
	this->texture = nullptr;
	this->textureView = nullptr;
}

Texture::Texture(const Texture&) 
{

}

Texture::~Texture() 
{

}

bool Texture::Initialize(ID3D11Device* device,ID3D11DeviceContext* deviceContext, char* filename, bool isTarga) 
{
	bool result;
	int height;
	int width;
	D3D11_TEXTURE2D_DESC textureDesc;
	HRESULT hresult;
	unsigned int rowPitch;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	std::string format;
	std::string finalPath;

	std::string path = "../Ze3DProject/Textures/";
	if (isTarga) {
		format = ".tga";
		finalPath = path + filename + format;

		//Load the targa image data into memory
		result = this->LoadTarga(finalPath.c_str(), height, width);
		if (!result) {
			return false;
		}

		//Setup the description of the texture
		textureDesc.Height = height;
		textureDesc.Width = width;
		textureDesc.MipLevels = 0;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

		//Create the empty texture
		hresult = device->CreateTexture2D(&textureDesc, NULL, &this->texture);
		if (FAILED(hresult)) {
			return false;
		}

		//Set the row pitch of the targa data
		rowPitch = (width * 4) * sizeof(unsigned char);

		//Copy the targa image data into the texture
		deviceContext->UpdateSubresource(this->texture, 0, NULL, this->targaData, rowPitch, 0);

		//Setup the shader resource view description
		srvDesc.Format = textureDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = -1;

		//Create the shader resource view for the texture
		hresult = device->CreateShaderResourceView(this->texture, &srvDesc, &this->textureView);
		if (FAILED(hresult)) {
			return false;
		}

		//Release the targa image data now that the image data has been loaded into the texture
		delete[] this->targaData;
		this->targaData = nullptr;
	}
	else {
		format = ".jpg";
		finalPath = path + filename + format;

	}

	//Generate mipmaps for this texture
	deviceContext->GenerateMips(this->textureView);

	return true;
}

void Texture::Shutdown() 
{
	//Release the texture view resource
	if (this->textureView) {
		this->textureView->Release();
		this->textureView = nullptr;
	}

	//Release the texture
	if (this->texture) {
		this->texture->Release();
		this->texture = nullptr;
	}

	//Release the targa data
	if (this->targaData) {
		delete[] this->targaData;
		this->targaData = nullptr;
	}

	return;
}

ID3D11ShaderResourceView* Texture::GetTexture() 
{
	return this->textureView;
}

bool Texture::LoadTarga(const char*filename, int&height, int& width) 
{
	int error, bpp, imageSize, index, i, j, k;
	FILE* filePtr;
	unsigned int count;
	TargaHeader targaFileHeader;
	unsigned char* targaImage;

	//Open the targa file for reading in binary
	error = fopen_s(&filePtr, filename, "rb");
	if (error != 0) {
		return false;
	}

	//Read in the file header
	count = (unsigned int)fread(&targaFileHeader, sizeof(TargaHeader), 1, filePtr);
	if (count != 1) {
		return false;
	}

	//Get the important information from the header
	height = (int)targaFileHeader.height;
	width = (int)targaFileHeader.width;
	bpp = (int)targaFileHeader.bpp;

	//Check that it is 32bit and not 24bit
	if (bpp != 32) {
		return false;
	}

	//Calculate the size of the 32 bit image data
	imageSize = width * height * 4;

	//Allocate memory for the targa image data
	targaImage = new unsigned char[imageSize];
	if (!targaImage) {
		return false;
	}

	//Read in the targa image data
	count = (unsigned int)fread(targaImage, 1, imageSize, filePtr);
	if (count != imageSize) {
		return false;
	}

	//Close the file
	error = fclose(filePtr);
	if (error != 0) {
		return false;
	}

	//Allocate memory for the targa destination data
	this->targaData = new unsigned char[imageSize];
	if (!this->targaData) {
		return false;
	}

	//Initialize the index into the targa destination data
	index = 0;

	//Initialize the index into the targa image data
	k = (width * height * 4) - (width * 4);

	//Now copy the targa imaga data into the targa destionation array int he correct order since the targa format is stored upside down
	for (j = 0; j < height; j++) {

		for (i = 0; i < width; i++) {
			this->targaData[index + 0] = targaImage[k + 2];	//Red
			this->targaData[index + 1] = targaImage[k + 1];	//Green
			this->targaData[index + 2] = targaImage[k + 0];	//Blue
			this->targaData[index + 3] = targaImage[k + 3];	//Alpha

			//Increment the indexes into the targa data
			k += 4;
			index += 4;
		}

		//Set the targa image index back to the preceding row at the begining of the coulum sinceits reding it in upside down
		k -= (width * 8);
	}

	//Release the targa image data now that it was copied into the destination array
	delete[] targaImage;
	targaImage = nullptr;

	return true;
}


