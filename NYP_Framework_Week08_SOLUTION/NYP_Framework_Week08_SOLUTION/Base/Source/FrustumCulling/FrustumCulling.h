#pragma once

#include "Vector3.h"
#include "Plane.h"
#include "SingletonTemplate.h"

class CFrustumCulling : public Singleton<CFrustumCulling>
{
	friend Singleton<CFrustumCulling>;
protected:
	enum {
		TOP, 
		BOTTOM, 
		LEFT,
		RIGHT, 
		NEARPLANE, 
		FARPLANE
	};

	CPlane planes[6];

	//n = near, f = far , t = top, l = left, r = right, b = bottom
	Vector3 ntl, ntr, nbl, nbr, ftl, ftr, fbl, fbr;
	float nearDist, farDist, ratio, angle, tang;
	float nearWidth, nearHeight, farWidth, farHeight;

public:
	enum 
	{ 
		OUTSIDE, 
		INTERSECT, 
		INSIDE 
	};

	// Constructor
	CFrustumCulling::CFrustumCulling(void);
	// Destructor
	CFrustumCulling::~CFrustumCulling(void);

	// Set methods
	//angle = Field of View, ratio = aspect ratio, CALL THIS FUNCTION IF RESOLUTION/FOV/VIEW DIST CHANGE
	// Initialise the frustum
	void Init(	const float angle, 
				const float ratio, 
				const float nearDist, 
				const float farDist);
	// Update the frustum
	void Update(	Vector3 pos, 
					Vector3 target, 
					Vector3 up);

	// Get methods
	// Get a positive vertex 
	Vector3 GetVertexPositive(	Vector3 pos, 
								const float xDist, 
								const float zDist, 
								Vector3 planeNormal);
	// Get a negative vertex 
	Vector3 GetVertexNegative(	Vector3 pos, 
								const float xDist, 
								const float zDist, 
								Vector3 planeNormal);

	// Check methods
	// Check if a point is in the Frustum
	bool isPointInFrustum(Vector3 pos);
	// Check if a sphere is in the Frustum by checking its centre and radius
	bool isSphereInFrustum(	Vector3 pos, 
							const float radius);
	// Check if a box is in the Frustum by checking its AABB
	bool isBoxInFrustum(Vector3 pos, 
						const float xDist, 
						const float zDist);
};
