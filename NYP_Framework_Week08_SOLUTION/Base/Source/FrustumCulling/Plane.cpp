#include "Plane.h"

// Constructor
CPlane::CPlane(void)
{
}

CPlane::CPlane(Vector3 pos1, Vector3 pos2, Vector3 pos3)
{
	Set3Points(pos1, pos2, pos3);
}

// Destructor
CPlane::~CPlane(void)
{
}

// Set methods
// Set Normal, point and constant term
void CPlane::Set3Points(Vector3 pos1, Vector3 pos2, Vector3 pos3)
{
	Vector3 vec1, vec2;

	//CPlane Normal = cross product of 2 direction vectors
	vec1 = pos1 - pos2; //BA
	vec2 = pos3 - pos2; //BC

	normal = vec2.Cross(vec1);
	normal.Normalize();
	point = pos2; //Set reference point
	d = -(normal.x * point.x + normal.y * point.y + normal.z * point.z); //Get Constant
}

// Get methods
//Get distance from CPlane
float CPlane::GetDist(Vector3 pos)
{
	return (d + (normal.x * pos.x + normal.y * pos.y + normal.z * pos.z));
}

// Get the normal of this plane
Vector3 CPlane::GetNormal(void)
{
	return normal;
}
