#ifndef FRUSTUM_H
#define FRUSTUM_H

#include <d3d11.h>
#include <DirectXMath.h>

using namespace DirectX;

class Frustum {

private:
	XMVECTOR planes[6];
	XMMATRIX viewMatrix;
public:
	Frustum();
	~Frustum();

	void SetViewMatrix(XMMATRIX viewMatrix);
	void CreateFrustum(float, XMMATRIX viewMatrix, XMMATRIX projMatrix);
	bool IntersectBB(XMVECTOR* boundingbox);
};

#endif