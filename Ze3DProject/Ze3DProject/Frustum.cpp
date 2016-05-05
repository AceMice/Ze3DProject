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

	XMMATRIX updatedProjMatrix = XMLoadFloat4x4(&projMatrixFloat);

	//Create the frustum from the viewMatrix and updated projectionMatrix
	XMStoreFloat4x4(&frustumMatrix, XMMatrixMultiply(viewMatrix, updatedProjMatrix));

	//Calculate near plane of frustum
	/*XMVectorSetX(this->planes[0], frustumMatrix._14 + frustumMatrix._13);
	XMVectorSetY(this->planes[0], frustumMatrix._24 + frustumMatrix._23);
	XMVectorSetZ(this->planes[0], frustumMatrix._34 + frustumMatrix._33);
	XMVectorSetW(this->planes[0], frustumMatrix._44 + frustumMatrix._43);*/
	this->planes[0] = XMVectorSet(frustumMatrix._14 + frustumMatrix._13, frustumMatrix._24 + frustumMatrix._23, frustumMatrix._34 + frustumMatrix._33, frustumMatrix._44 + frustumMatrix._43);
	this->planes[0] = XMPlaneNormalize(this->planes[0]);
	//this->planes[0] = XMVector3Normalize(this->planes[0]);

	//Calculate far plane of frustum
	/*XMVectorSetX(this->planes[1], frustumMatrix._14 - frustumMatrix._13);
	XMVectorSetY(this->planes[1], frustumMatrix._24 - frustumMatrix._23);
	XMVectorSetZ(this->planes[1], frustumMatrix._34 - frustumMatrix._33);
	XMVectorSetW(this->planes[1], frustumMatrix._44 - frustumMatrix._43);*/
	this->planes[1] = XMVectorSet(frustumMatrix._14 - frustumMatrix._13, frustumMatrix._24 - frustumMatrix._23, frustumMatrix._34 - frustumMatrix._33, frustumMatrix._44 - frustumMatrix._43);
	this->planes[1] = XMPlaneNormalize(this->planes[1]);
	//this->planes[1] = XMVector3Normalize(this->planes[1]);

	//Calculate left plane of frustum
	/*XMVectorSetX(this->planes[2], frustumMatrix._14 + frustumMatrix._11);
	XMVectorSetY(this->planes[2], frustumMatrix._24 + frustumMatrix._21);
	XMVectorSetZ(this->planes[2], frustumMatrix._34 + frustumMatrix._31);
	XMVectorSetW(this->planes[2], frustumMatrix._44 + frustumMatrix._41);*/
	this->planes[2] = XMVectorSet(frustumMatrix._14 + frustumMatrix._11, frustumMatrix._24 + frustumMatrix._21, frustumMatrix._34 + frustumMatrix._31, frustumMatrix._44 + frustumMatrix._41);
	this->planes[2] = XMPlaneNormalize(this->planes[2]);
	//this->planes[2] = XMVector3Normalize(this->planes[2]);

	//Calculate right plane of frustum
	/*XMVectorSetX(this->planes[3], frustumMatrix._14 - frustumMatrix._11);
	XMVectorSetY(this->planes[3], frustumMatrix._24 - frustumMatrix._21);
	XMVectorSetZ(this->planes[3], frustumMatrix._34 - frustumMatrix._31);
	XMVectorSetW(this->planes[3], frustumMatrix._44 - frustumMatrix._41);*/
	this->planes[3] = XMVectorSet(frustumMatrix._14 - frustumMatrix._11, frustumMatrix._24 - frustumMatrix._21, frustumMatrix._34 - frustumMatrix._31, frustumMatrix._44 - frustumMatrix._41);
	this->planes[3] = XMPlaneNormalize(this->planes[3]);
	//this->planes[3] = XMVector3Normalize(this->planes[3]);

	//Calculate top plane of frustum
	/*XMVectorSetX(this->planes[4], frustumMatrix._14 - frustumMatrix._12);
	XMVectorSetY(this->planes[4], frustumMatrix._24 - frustumMatrix._22);
	XMVectorSetZ(this->planes[4], frustumMatrix._34 - frustumMatrix._32);
	XMVectorSetW(this->planes[4], frustumMatrix._44 - frustumMatrix._42);*/
	this->planes[4] = XMVectorSet(frustumMatrix._14 - frustumMatrix._12, frustumMatrix._24 - frustumMatrix._22, frustumMatrix._34 - frustumMatrix._32, frustumMatrix._44 - frustumMatrix._42);
	this->planes[4] = XMPlaneNormalize(this->planes[4]);
	//this->planes[4] = XMVector3Normalize(this->planes[4]);

	//Calculate bottom plane of frustum
	//XMVectorSetX(this->planes[5], frustumMatrix._14 + frustumMatrix._12);
	//XMVectorSetY(this->planes[5], frustumMatrix._24 + frustumMatrix._22);
	//XMVectorSetZ(this->planes[5], frustumMatrix._34 + frustumMatrix._32);
	//XMVectorSetW(this->planes[5], frustumMatrix._44 + frustumMatrix._42);
	this->planes[5] = XMVectorSet(frustumMatrix._14 + frustumMatrix._12, frustumMatrix._24 + frustumMatrix._22, frustumMatrix._34 + frustumMatrix._32, frustumMatrix._44 + frustumMatrix._42);
	this->planes[5] = XMPlaneNormalize(this->planes[5]);
	//this->planes[5] = XMVector3Normalize(this->planes[5]);

	/*for (int i = 0; i < 6; i++) {
		float denom = 1.0f / XMVectorGetX(XMVector3Length(this->planes[i]));
		XMVectorSetX(this->planes[i], XMVectorGetX(this->planes[i]) * denom);
		XMVectorSetY(this->planes[i], XMVectorGetY(this->planes[i]) * denom);
		XMVectorSetZ(this->planes[i], XMVectorGetZ(this->planes[i]) * denom);
		XMVectorSetW(this->planes[i], XMVectorGetW(this->planes[i]) * denom);
	}*/

	return;
}

bool Frustum::IntersectBB(XMVECTOR* boundingBox)
{
	XMVECTOR* p1;
	XMVECTOR* p2;
	bool pointInside = false;
	int outside = 0;

	for (int i = 0; i < 8; i++) { //Loop through all the corner points for bounding box
		for (int j = 0; j < 6; j++) { //Check if any point is inside all frustum planes
			/*if (XMVectorGetX(this->planes[j]) * XMVectorGetX(boundingBox[i]) + XMVectorGetY(this->planes[j]) * XMVectorGetY(boundingBox[i]) + XMVectorGetZ(this->planes[j]) * XMVectorGetZ(boundingBox[i]) <= 0) {
				outside++;
			}*/
			if (XMVectorGetX(XMPlaneDotCoord(this->planes[j], boundingBox[i])) <= 0.0f) {
				outside++;
			}
			/*if (XMVectorGetX(XMVector3Dot(this->planes[j], boundingBox[i])) <= 0.0f) {
				outside++;
			}*/
		}
		if (outside < 2) { //If any point is inside frustum -> return intersection
			return true;
		}
		outside = 0;
	}

	/*for (int i = 0; i < 6; i++) {
		if (XMVectorGetX(XMPlaneDotCoord(this->planes[i], boundingBox[0])) >= 0.0f) {
			continue;
		}
		if (XMVectorGetX(XMPlaneDotCoord(this->planes[i], boundingBox[1])) >= 0.0f) {
			continue;
		}
		if (XMVectorGetX(XMPlaneDotCoord(this->planes[i], boundingBox[2])) >= 0.0f) {
			continue;
		}
		if (XMVectorGetX(XMPlaneDotCoord(this->planes[i], boundingBox[3])) >= 0.0f) {
			continue;
		}
		if (XMVectorGetX(XMPlaneDotCoord(this->planes[i], boundingBox[4])) >= 0.0f) {
			continue;
		}
		if (XMVectorGetX(XMPlaneDotCoord(this->planes[i], boundingBox[5])) >= 0.0f) {
			continue;
		}
		if (XMVectorGetX(XMPlaneDotCoord(this->planes[i], boundingBox[6])) >= 0.0f) {
			continue;
		}
		if (XMVectorGetX(XMPlaneDotCoord(this->planes[i], boundingBox[7])) >= 0.0f) {
			continue;
		}
		return false;
	}*/

	return false;
}