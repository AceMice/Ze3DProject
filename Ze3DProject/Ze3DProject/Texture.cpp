#include "Texture.h"


Texture::Texture() 
{
	this->targaData = nullptr;
	this->texture = nullptr;
}

Texture::Texture(const Texture&) 
{

}

Texture::~Texture() 
{

}

bool Texture::Initialize(ID3D11Device* device,ID3D11DeviceContext* deviceContext, std::string materialLib)
{
	bool result;

	result = this->LoadMtl(device, deviceContext, materialLib);
	if (!result) {
		return false;
	}

	for (int i = 0; i < this->textureViews.size(); i++) {
		//Generate mipmaps for this texture
		deviceContext->GenerateMips(this->textureViews.at(i));
	}
	

	return true;
}

void Texture::Shutdown() 
{
	//Release the texture view resource
	for (int i = 0; i < this->textureViews.size(); i++) {
		this->textureViews.at(i)->Release();
		this->textureViews.at(i) = nullptr;
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

ID3D11ShaderResourceView* Texture::GetTexture(int textureIndex) 
{
	if (textureIndex == -1) {
		return NULL;
	}
	if (this->textureViews.size() > textureIndex) {
		return this->textureViews.at(textureIndex);
	}
	//Safe return just in case
	return NULL;
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

bool Texture::LoadMtl(ID3D11Device* device, ID3D11DeviceContext* deviceContext, std::string materialLib)
{
	std::ifstream file;
	std::string line;
	std::string junk;
	std::stringstream ss;
	int nrOfMaterials = 0;
	bool difSet;
	bool firstTextureLoad = true;
	int height;
	int width;
	D3D11_TEXTURE2D_DESC textureDesc;
	HRESULT hresult;
	unsigned int rowPitch;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	std::string path = "../Ze3DProject/Textures/";
	std::string finalPath;
	std::string mtlPath = "../Ze3DProject/OBJ/";
	std::string finalMtlPath = mtlPath + materialLib;
	bool result;

	file.open(finalMtlPath);
	if (!file.is_open()) {
		return false;
	}

	while (std::getline(file, line)) {
		if (line.size() > 0) {
			if (line.substr(0, 6) == "newmtl") {
				ss.clear();
				ss.str(line);
				Material tempMat;
				this->materials.push_back(tempMat);
				ss >> junk >> this->materials.at(nrOfMaterials).name;
				this->materials.at(nrOfMaterials).hasTexture = false;
				this->materials.at(nrOfMaterials).textureIndex = 0;
				this->materials.at(nrOfMaterials).specColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
				this->materials.at(nrOfMaterials).transparent = false;
				nrOfMaterials++;
				difSet = false;
			}
			else if(line.at(0) == 'K'){
				ss.clear();
				ss.str(line);
				if (line.at(1) == 'd') {
					ss >> junk >> this->materials.at(nrOfMaterials - 1).difColor.x
						>> this->materials.at(nrOfMaterials - 1).difColor.y
						>> this->materials.at(nrOfMaterials - 1).difColor.z;
					difSet = true;
				}
				else if (!difSet && line.at(1) == 'a') {
					ss >> junk >> this->materials.at(nrOfMaterials - 1).difColor.x
						>> this->materials.at(nrOfMaterials - 1).difColor.y
						>> this->materials.at(nrOfMaterials - 1).difColor.z;
					difSet = true;
				}
				else if (line.at(1) == 's') {
					ss >> junk >> this->materials.at(nrOfMaterials - 1).specColor.x
						>> this->materials.at(nrOfMaterials - 1).specColor.y
						>> this->materials.at(nrOfMaterials - 1).specColor.z;
				}
			}
			else if (line.at(0) == 'T') {
				if (line.at(1) == 'f') {
					float trans = 0.0f;
					ss.clear();
					ss.str(line);
					ss >> junk >> trans;
					trans = 1.0f - trans;
					if (trans > 0.0f) {
						this->materials.at(nrOfMaterials - 1).difColor.w = trans;
						this->materials.at(nrOfMaterials - 1).transparent = true;
					}
				}
			}
			else if (line.substr(0, 6) == "map_Kd") {
				if (line.size() > 8) {
					bool beenLoaded = false;
					std::string textureFilename;
					ss.clear();
					ss.str(line);
					ss >> junk >> textureFilename;

					for (int i = 0; !beenLoaded && i < this->textureNames.size(); i++) {
						if (this->textureNames.at(i) == textureFilename) {
							this->materials.at(nrOfMaterials - 1).hasTexture = true;
							this->materials.at(nrOfMaterials - 1).textureIndex = i;
							beenLoaded = true;
						}
					}
					if (!beenLoaded) {
						//Load the targa image data into memory
						finalPath = path + textureFilename;
						result = this->LoadTarga(finalPath.c_str(), height, width);
						if (!result) {
							return false;
						}

						//If it's the first texture, init some general desc values
						if (firstTextureLoad) {
							//Setup the description of the texture
							textureDesc.MipLevels = 0;
							textureDesc.ArraySize = 1;
							textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
							textureDesc.SampleDesc.Count = 1;
							textureDesc.SampleDesc.Quality = 0;
							textureDesc.Usage = D3D11_USAGE_DEFAULT;
							textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
							textureDesc.CPUAccessFlags = 0;
							textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

							//Setup the shader resource view description
							srvDesc.Format = textureDesc.Format;
							srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
							srvDesc.Texture2D.MostDetailedMip = 0;
							srvDesc.Texture2D.MipLevels = -1;
						}
						//Set the teexture specific description values
						textureDesc.Height = height;
						textureDesc.Width = width;

						//Create the empty texture
						hresult = device->CreateTexture2D(&textureDesc, NULL, &this->texture);
						if (FAILED(hresult)) {
							return false;
						}

						//Set the row pitch of the targa data
						rowPitch = (width * 4) * sizeof(unsigned char);

						//Copy the targa image data into the texture
						deviceContext->UpdateSubresource(this->texture, 0, NULL, this->targaData, rowPitch, 0);

						//Create the shader resource view for the texture
						ID3D11ShaderResourceView* tempTextureView;

						hresult = device->CreateShaderResourceView(this->texture, &srvDesc, &tempTextureView);
						if (FAILED(hresult)) {
							return false;
						}
						this->textureNames.push_back(textureFilename);
						this->materials.at(nrOfMaterials - 1).hasTexture = true;
						this->materials.at(nrOfMaterials - 1).textureIndex = this->textureViews.size();
						this->textureViews.push_back(tempTextureView);

						//Release the texture
						if (this->texture) {
							this->texture->Release();
							this->texture = nullptr;
						}
						//Release the targa image data now that the image data has been loaded into the texture
						delete[] this->targaData;
						this->targaData = nullptr;
					}
				}
				
			}
		}
	}
	file.close();

	return true;
}

Texture::Material Texture::GetMaterial(std::string materialName)
{
	for (int i = 0; i < this->materials.size(); i++) {
		if (this->materials.at(i).name == materialName) {
			return this->materials.at(i);
		}
	}
	Material defaultMaterial;
	defaultMaterial.name = "Not found";
	defaultMaterial.difColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	defaultMaterial.hasTexture = false;
	defaultMaterial.textureIndex = -1;

	return defaultMaterial;
}