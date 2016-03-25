#include "Frustum.h"

Frustum::Frustum()
{
}
Frustum::~Frustum()
{
}

void Frustum::CreateFrustum(float screenDepth, XMMATRIX viewMatrix, XMMATRIX projMatrix)
{
	float zMinimum, r;
	XMFLOAT4X4 projMatrixFloat;
	XMFLOAT4X4 viewProjMatrixFloat;
	XMStoreFloat4x4(&projMatrixFloat, projMatrix);

	// Calculate the minimum Z distance in the frustum.
	zMinimum = -projMatrixFloat._43 / projMatrixFloat._33;
	r = screenDepth / (screenDepth - zMinimum);
	projMatrixFloat._33 = r;
	projMatrixFloat._43 = -r * zMinimum;

	// Create the frustum viewProjMatrixFloat from the view viewProjMatrixFloat and updated projection viewProjMatrixFloat.
	XMStoreFloat4x4(&viewProjMatrixFloat, XMMatrixMultiply(viewMatrix, projMatrix)); ;

	// Calculate near plane of frustum.
	XMVectorSetX(this->planes[0], viewProjMatrixFloat._14 + viewProjMatrixFloat._13);
	XMVectorSetY(this->planes[0], viewProjMatrixFloat._24 + viewProjMatrixFloat._23);
	XMVectorSetZ(this->planes[0], viewProjMatrixFloat._34 + viewProjMatrixFloat._33);
	XMVectorSetW(this->planes[0], viewProjMatrixFloat._44 + viewProjMatrixFloat._43);
	this->planes[0] = XMPlaneNormalize(this->planes[0]);
	
	// Calculate far plane of frustum.
	XMVectorSetX(this->planes[1], viewProjMatrixFloat._14 - viewProjMatrixFloat._13);
	XMVectorSetY(this->planes[1], viewProjMatrixFloat._24 - viewProjMatrixFloat._23);
	XMVectorSetZ(this->planes[1], viewProjMatrixFloat._34 - viewProjMatrixFloat._33);
	XMVectorSetW(this->planes[1], viewProjMatrixFloat._44 - viewProjMatrixFloat._43);
	this->planes[1] = XMPlaneNormalize(this->planes[1]);

	// Calculate left plane of frustum.
	XMVectorSetX(this->planes[2], viewProjMatrixFloat._14 + viewProjMatrixFloat._11);
	XMVectorSetY(this->planes[2], viewProjMatrixFloat._24 + viewProjMatrixFloat._21);
	XMVectorSetZ(this->planes[2], viewProjMatrixFloat._34 + viewProjMatrixFloat._31);
	XMVectorSetW(this->planes[2], viewProjMatrixFloat._44 + viewProjMatrixFloat._41);
	this->planes[2] = XMPlaneNormalize(this->planes[2]);

	// Calculate right plane of frustum.
	XMVectorSetX(this->planes[3], viewProjMatrixFloat._14 - viewProjMatrixFloat._11);
	XMVectorSetY(this->planes[3], viewProjMatrixFloat._24 - viewProjMatrixFloat._21);
	XMVectorSetZ(this->planes[3], viewProjMatrixFloat._34 - viewProjMatrixFloat._31);
	XMVectorSetW(this->planes[3], viewProjMatrixFloat._44 - viewProjMatrixFloat._41);
	this->planes[3] = XMPlaneNormalize(this->planes[3]);

	// Calculate top plane of frustum.
	XMVectorSetX(this->planes[4], viewProjMatrixFloat._14 - viewProjMatrixFloat._12);
	XMVectorSetY(this->planes[4], viewProjMatrixFloat._24 - viewProjMatrixFloat._22);
	XMVectorSetZ(this->planes[4], viewProjMatrixFloat._34 - viewProjMatrixFloat._32);
	XMVectorSetW(this->planes[4], viewProjMatrixFloat._44 - viewProjMatrixFloat._42);
	this->planes[4] = XMPlaneNormalize(this->planes[4]);

	// Calculate bottom plane of frustum.
	XMVectorSetX(this->planes[5], viewProjMatrixFloat._14 + viewProjMatrixFloat._12);
	XMVectorSetY(this->planes[5], viewProjMatrixFloat._24 + viewProjMatrixFloat._22);
	XMVectorSetZ(this->planes[5], viewProjMatrixFloat._34 + viewProjMatrixFloat._32);
	XMVectorSetW(this->planes[5], viewProjMatrixFloat._44 + viewProjMatrixFloat._42);
	this->planes[5] = XMPlaneNormalize(this->planes[5]);

	return;
}

bool Frustum::IntersectBB(XMVECTOR boundingBox[8])
{
	XMVECTOR* p1;
	XMVECTOR* p2;
	bool pointInside = false;
	int planesIntersected = 0;
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 8 && !pointInside; j++) {
			if (XMVectorGetX(XMPlaneDotCoord(this->planes[i], boundingBox[j])) >= 0.0f) {
				pointInside = true;
			}
		}
		if (!pointInside) {
			return false;
		}
		pointInside = false;
	}
	return true;
}