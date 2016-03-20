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
	struct HeightMapInfo {
		int terrainWidth;
		int terrainHeight;
		XMFLOAT3 *heightMap;
	};

public:

	bool HeightMapLoad(char*, HeightMapInfo&);

};


#endif