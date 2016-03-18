#include "HeightMap.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <windows.h>

bool HeightMap::HeightMapLoad(char* filename, HeightMapInfo& hminfo) {
	FILE* filePtr;				//Point to current position in file
	BITMAPFILEHEADER bitmapFileHeader;	//Struct which stores infrmation about the file
	BITMAPINFOHEADER bitmapInfoHeader;	//Struct which stores information about the image
	int imageSize;
	int index;
	unsigned char height;
	
	//Add .bmp
	std::string fullFileName = filename;
	fullFileName += ".bmp";
	filePtr = fopen(fullFileName.c_str(), "rb");

	//Check if open
	if (filePtr == NULL) {
		return 0;
	}

	//Read bitmap header
	fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);

	//Read bitmap info header
	fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);

	//Get the width and height of the image
	hminfo.terrainWidth = bitmapInfoHeader.biWidth;
	hminfo.terrainHeight = bitmapInfoHeader.biHeight;

	//Size of the image in bytes. The 3 represent RGB (byte, byte, byte) for each pixel
	imageSize = hminfo.terrainWidth * hminfo.terrainHeight * 3;
	
	unsigned char* bitmapImage = new unsigned char[imageSize];
	
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

	// READ FROM HERE LATER //

	// Store image data in bitmapImage
	fread(bitmapImage, 1, imageSize, filePtr);

	// Close file
	fclose(filePtr);

	// Initialize the heightMap array (stores the vertices of our terrain)
	hminfo.heightMap = new XMFLOAT3[hminfo.terrainWidth * hminfo.terrainHeight];

	// We use a greyscale image, so all 3 rgb values are the same, but we only need one for the height
	// So we use this counter to skip the next two components in the image data (we read R, then skip BG)
	int k = 0;

	// We divide the height by this number to "water down" the terrains height, otherwise the terrain will
	// appear to be "spikey" and not so smooth.
	float heightFactor = 10.0f;

	// Read the image data into our heightMap array
	for (int j = 0; j< hminfo.terrainHeight; j++)
	{
		for (int i = 0; i< hminfo.terrainWidth; i++)
		{
			height = bitmapImage[k];

			index = (hminfo.terrainHeight * j) + i;

			hminfo.heightMap[index].x = (float)i;
			hminfo.heightMap[index].y = (float)height / heightFactor;
			hminfo.heightMap[index].z = (float)j;

			k += 3;
		}
	}

	delete[] bitmapImage;
	bitmapImage = 0;

	return true;
}
}