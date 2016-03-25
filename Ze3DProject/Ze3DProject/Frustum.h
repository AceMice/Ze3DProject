#ifndef FRUSTUM_H
#define FRUSTUM_H

#include <d3d11.h>
#include <DirectXMath.h>

using namespace DirectX;

class Frustum {

private:
	XMVECTOR planes[6];
public:
	Frustum();
	~Frustum();

	void CreateFrustum(float, XMMATRIX, XMMATRIX);
	bool IntersectBB(XMVECTOR[8]);
};

#endif