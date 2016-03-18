#ifndef HEIGHTMAP_H
#define HEIGHTMAP_H

#include <DirectXMath.h>
using namespace DirectX;

class HightMap {

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