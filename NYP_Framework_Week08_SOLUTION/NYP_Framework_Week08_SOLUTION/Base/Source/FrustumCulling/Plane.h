#pragma once

#include "Vector3.h"

class CPlane
{
protected:
	//CPlane is defined by a normal(e.g 2x - y + 1 = 0) and a point on the CPlane
	Vector3 normal, point;
	//Constant term, d = normal.x * point.x + normal.y + point.y + normal.z * point.z
	float d;

public:
	// Constructor
	CPlane::CPlane(void);
	CPlane::CPlane(Vector3 pos1, Vector3 pos2, Vector3 pos3);
	// Destructor
	CPlane::~CPlane(void);

	// Set methods
	// Set Normal, point and constant term from 3 points
	void Set3Points(Vector3 pos1, Vector3 pos2, Vector3 pos3);

	// Get methods
	// Get distance from CPlane
	float GetDist(Vector3 pos);
	// Get the normal of this plane
	Vector3 GetNormal(void);
};
