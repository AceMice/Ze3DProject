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
	XMFLOAT4X4 frustumMatrix;
	XMStoreFloat4x4(&projMatrixFloat, projMatrix);

	// Calculate the minimum Z distance in the frustum.
	zMinimum = -projMatrixFloat._43 / projMatrixFloat._33;
	r = screenDepth / (screenDepth - zMinimum);
	projMatrixFloat._33 = r;
	projMatrixFloat._43 = -r * zMinimum;

	// Create the frustum from the viewMatrix and updated projectionMatrix.
	XMStoreFloat4x4(&frustumMatrix, XMMatrixMultiply(viewMatrix, projMatrix)); ;

	// Calculate near plane of frustum.
	XMVectorSetX(this->planes[0], frustumMatrix._14 + frustumMatrix._13);
	XMVectorSetY(this->planes[0], frustumMatrix._24 + frustumMatrix._23);
	XMVectorSetZ(this->planes[0], frustumMatrix._34 + frustumMatrix._33);
	XMVectorSetW(this->planes[0], frustumMatrix._44 + frustumMatrix._43);
	//this->planes[0] = XMPlaneNormalize(this->planes[0]);
	this->planes[0] = XMVector4Normalize(this->planes[0]);

	// Calculate far plane of frustum.
	XMVectorSetX(this->planes[1], frustumMatrix._14 - frustumMatrix._13);
	XMVectorSetY(this->planes[1], frustumMatrix._24 - frustumMatrix._23);
	XMVectorSetZ(this->planes[1], frustumMatrix._34 - frustumMatrix._33);
	XMVectorSetW(this->planes[1], frustumMatrix._44 - frustumMatrix._43);
	//this->planes[1] = XMPlaneNormalize(this->planes[1]);
	this->planes[1] = XMVector4Normalize(this->planes[1]);

	// Calculate left plane of frustum.
	XMVectorSetX(this->planes[2], frustumMatrix._14 + frustumMatrix._11);
	XMVectorSetY(this->planes[2], frustumMatrix._24 + frustumMatrix._21);
	XMVectorSetZ(this->planes[2], frustumMatrix._34 + frustumMatrix._31);
	XMVectorSetW(this->planes[2], frustumMatrix._44 + frustumMatrix._41);
	//this->planes[2] = XMPlaneNormalize(this->planes[2]);
	this->planes[2] = XMVector4Normalize(this->planes[2]);

	// Calculate right plane of frustum.
	XMVectorSetX(this->planes[3], frustumMatrix._14 - frustumMatrix._11);
	XMVectorSetY(this->planes[3], frustumMatrix._24 - frustumMatrix._21);
	XMVectorSetZ(this->planes[3], frustumMatrix._34 - frustumMatrix._31);
	XMVectorSetW(this->planes[3], frustumMatrix._44 - frustumMatrix._41);
	//this->planes[3] = XMPlaneNormalize(this->planes[3]);
	this->planes[3] = XMVector4Normalize(this->planes[3]);

	// Calculate top plane of frustum.
	XMVectorSetX(this->planes[4], frustumMatrix._14 - frustumMatrix._12);
	XMVectorSetY(this->planes[4], frustumMatrix._24 - frustumMatrix._22);
	XMVectorSetZ(this->planes[4], frustumMatrix._34 - frustumMatrix._32);
	XMVectorSetW(this->planes[4], frustumMatrix._44 - frustumMatrix._42);
	//this->planes[4] = XMPlaneNormalize(this->planes[4]);
	this->planes[4] = XMVector4Normalize(this->planes[4]);

	// Calculate bottom plane of frustum.
	XMVectorSetX(this->planes[5], frustumMatrix._14 + frustumMatrix._12);
	XMVectorSetY(this->planes[5], frustumMatrix._24 + frustumMatrix._22);
	XMVectorSetZ(this->planes[5], frustumMatrix._34 + frustumMatrix._32);
	XMVectorSetW(this->planes[5], frustumMatrix._44 + frustumMatrix._42);
	//this->planes[5] = XMPlaneNormalize(this->planes[5]);
	this->planes[5] = XMVector4Normalize(this->planes[5]);

	return;
}

bool Frustum::IntersectBB(XMVECTOR* boundingBox)
{
	XMVECTOR* p1;
	XMVECTOR* p2;
	bool pointInside = false;
	int insideAll = 0;

	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 6; j++) {
			if (XMVectorGetX(XMPlaneDotCoord(this->planes[j], boundingBox[i])) >= 0.0f) {
				insideAll++;
			}
		}
		if (insideAll == 6) {
			return true;
		}
		insideAll = 0;
	}
	return false;
}