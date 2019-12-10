#include "FrustumCulling.h"

// Constructor
CFrustumCulling::CFrustumCulling()
{
}

// Destructor
CFrustumCulling::~CFrustumCulling()
{
}

// Set methods
// Initialise the frustum
void CFrustumCulling::Init(	const float angle, 
							const float ratio, 
							const float nearDist, 
							const float farDist)
{
	this->ratio = ratio;
	this->angle = angle;
	this->nearDist = nearDist;
	this->farDist = farDist;

	tang = (float)tan(angle * (Math::PI / 180));
	nearHeight = nearDist * tang;
	nearWidth = nearHeight * ratio;
	farHeight = farDist * tang;
	farWidth = farHeight * ratio;
}

// Update the frustum
void CFrustumCulling::Update(	Vector3 pos, 
								Vector3 target, 
								Vector3 up)
{
	//x = right, y = up, z = direction
	Vector3 nearCenter, farCenter, x, y, z;
	Vector3 nearHeightVec3, nearWidthVec3, farHeightVec3, farWidthVec3;

	z = pos - target;
	z.Normalize();

	x = up.Cross(z);
	x.Normalize();

	y = z.Cross(x);

	nearCenter = pos - z * nearDist;
	farCenter = pos - z * farDist;

	nearHeightVec3 = y * nearHeight;
	nearWidthVec3 = x * nearWidth;
	farHeightVec3 = y * farHeight;
	farWidthVec3 = x * farWidth;

	ntl = nearCenter + nearHeightVec3 * 0.5f - nearWidthVec3 * 0.5f;
	ntr = ntl + nearWidthVec3;
	nbl = ntl - nearHeightVec3;
	nbr = nbl + nearWidthVec3;

	ftl = farCenter + farHeightVec3 * 0.5f - farWidthVec3 * 0.5f;
	ftr = ftl + farWidthVec3;
	fbl = ftl - farHeightVec3;
	fbr = fbl + farWidthVec3;

	//Assign in counter clockwise order(so plane faces inwards)
	planes[TOP].Set3Points(ntr, ntl, ftl);
	planes[BOTTOM].Set3Points(nbl, nbr, fbr);
	planes[LEFT].Set3Points(ntl, nbl, fbl);
	planes[RIGHT].Set3Points(nbr, ntr, fbr);
	planes[NEARPLANE].Set3Points(ntl, ntr, nbr);
	planes[FARPLANE].Set3Points(ftr, ftl, fbl);
}

// Get methods
// Get a positive vertex 
Vector3 CFrustumCulling::GetVertexPositive(	Vector3 pos, 
											const float xDist, 
											const float zDist, 
											Vector3 planeNormal)
{
	//yDist is 1000
	Vector3 min(pos.x - xDist * 0.5f, 0, pos.z - zDist * 0.5f);

	if (planeNormal.x > 0)
		min.x += xDist;

	if (planeNormal.y > 0)
		min.y += 1000;

	if (planeNormal.z > 0)
		min.z += zDist;

	return min;
}

// Get a negative vertex 
Vector3 CFrustumCulling::GetVertexNegative(	Vector3 pos, 
											const float xDist, 
											const float zDist, 
											Vector3 planeNormal)
{
	//yDist is 1000
	Vector3 max(pos.x + xDist * 0.5f, 1000, pos.z + zDist * 0.5f);

	if (planeNormal.x > 0)
		max.x -= xDist;

	if (planeNormal.y > 0)
		max.y -= 1000;

	if (planeNormal.z > 0)
		max.z -= zDist;

	return max;
}

// Check methods
// Check if a point is in the Frustum
bool CFrustumCulling::isPointInFrustum(Vector3 pos)
{
	int result = INSIDE;
	for (int i = 0; i < 6; ++i) 
	{
		if (planes[i].GetDist(pos) < 0)
			return OUTSIDE;
	}
	return result;
}

// Check if a sphere is in the Frustum by checking its centre and radius
bool CFrustumCulling::isSphereInFrustum(Vector3 pos, 
										const float radius)
{
	int result = INSIDE;
	float distance;

	for (int i = 0; i < 6; ++i) 
	{
		distance = planes[i].GetDist(pos);

		if (distance < -radius)
			return OUTSIDE;

		else if (distance < radius)
			result = INTERSECT;
	}
	return result;
}

// Check if a box is in the Frustum by checking its AABB
bool CFrustumCulling::isBoxInFrustum(	Vector3 pos, 
										const float xDist,
										const float zDist)
{
	int result = INSIDE;
	for (int i = 0; i < 6; ++i) 
	{
		if (planes[i].GetDist(GetVertexPositive(pos, xDist, zDist, planes[i].GetNormal())) < 0)
			return OUTSIDE;
		else if (planes[i].GetDist(GetVertexNegative(pos, xDist, zDist, planes[i].GetNormal())) < 0)
			result = INTERSECT;
	}
	return result;

}
