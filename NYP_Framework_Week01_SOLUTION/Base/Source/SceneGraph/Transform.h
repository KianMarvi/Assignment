#pragma once
#include "Mtx44.h"

class CTransform
{
public:
	enum AXIS
	{
		X_AXIS = 0,
		Y_AXIS,
		Z_AXIS,
		NUM_AXIS
	};

	// Default Constructor
	CTransform(void);
	// Overloaded Constructor
	CTransform( const float dx, const float dy, const float dz  );
	// Destructor
	~CTransform(void);

	// Apply a translation to the Transformation Matrix
	void ApplyTranslate( const float dx, const float dy, const float dz  );
	// Get the translation from the Transformation Matrix
	void GetTranslate(float& x, float& y, float& z);
	// Get the x-axis translation from the Transformation Matrix
	float GetTranslate_X(void) const;
	// Get the y-axis translation from the Transformation Matrix
	float GetTranslate_Y(void) const;
	// Get the z-axis translation from the Transformation Matrix
	float GetTranslate_Z(void) const;
	// Set the translation
	void SetTranslate(float x, float y, float z);
	// Set the translation
	void SetTranslate(Vector3 theTranslateVector);
	// Apply a rotation to the Transformation Matrix
	void ApplyRotate(const float angle, const float rx, const float ry, const float rz);
	// Get the rotation from the Transformation Matrix
	float GetRotate(const AXIS theAxis) const;
	// Set the scale of the Transformation Matrix
	void SetScale(const float sx, const float sy, const float sz);
	// Set the scale of the Transformation Matrix
	void SetScale(Vector3 theScaleVector);
	// Get the scale from the Transformation Matrix
	void GetScale(float& x, float& y, float& z) const;
	// Get the x-axis scale from the Transformation Matrix
	float GetScale_X(void) const;
	// Get the y-axis scale from the Transformation Matrix
	float GetScale_Y(void) const;
	// Get the z-axis scale from the Transformation Matrix
	float GetScale_Z(void) const;

	// Apply a Transformation Matrix to the Transformation Matrix here
	void ApplyTransform(Mtx44 newMTX);

	// Reset the transformation matrix to identity matrix
	void Reset (void); //reset to identity
	
	// Get the transformation matrix
	Mtx44 GetTransform(void) const;
	// Get the inverse of the transformation matrix
	Mtx44 GetInverseTransform(void) const;

	// Print Self
	void PrintSelf(void) const;

protected:
	Mtx44 Mtx, DefaultMtx;
};
