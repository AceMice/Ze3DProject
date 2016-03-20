#ifndef HEIGHTMAP_H
#define HEIGHTMAP_H

#include <DirectXMath.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <windows.h>
using namespace DirectX;

class HeightMap {

private:


public:
	struct HeightMapInfo {
		int terrainWidth;
		int terrainHeight;
		XMFLOAT3 *heightMap;
	};

	HeightMap() {};
	~HeightMap() {};

	bool HeightMapLoad(char*, HeightMapInfo&);

};


#endif