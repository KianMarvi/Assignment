#include "Laser.h"

#include "MeshBuilder.h"
#include "../EntityManager.h"
#include "GraphicsManager.h"
#include "RenderHelper.h"
#include "GL\glew.h"

#include <iostream>
using namespace std;

CLaser::CLaser(void)
	: CProjectile(NULL)
	, m_fLength(1.0f)
	, angle_x(0.0)
	, angle_y(0.0)
	, angle_z(0.0)
{
}

CLaser::CLaser(Mesh* _modelMesh)
	: CProjectile(_modelMesh)
	, m_fLength(1.0f)
	, angle_x(0.0)
	, angle_y(0.0)
	, angle_z(0.0)
{
}

CLaser::~CLaser(void)
{
	modelMesh = NULL;
	theSource = NULL;
}

// Set the length of the laser
void CLaser::SetLength(const float m_fLength)
{
	this->m_fLength = m_fLength;
}

// Get the length of the laser
float CLaser::GetLength(void) const
{
	return m_fLength;
}

// Update this laser
void CLaser::CalculateAngles(void)
{
	// Calculate the angle of the laser with respect to the x-z plane
	angle_y = asin(theDirection.y / theDirection.Length());

	// Calculate the length of the laser when it is projected onto the x-z plane
	float theDirection_LengthInXYPlane = (float)(cos(angle_y) * theDirection.Length());

	// Calculate the angle of the laser with respect from the x-axis (1st Quadrant)
	angle_x = acos(theDirection.x / theDirection_LengthInXYPlane);

	// If the laser is going to the 3rd and 4th Quadrant, 
	// then we need to calculate the angle from x-axis in the clockwise direction
	if (theDirection.z >= 0)
		angle_x = (2 * Math::PI) - angle_x;

	// Calculate the angle of the laser from the z axis.
	angle_z = acos(theDirection.z / theDirection_LengthInXYPlane);
}

// Update the status of this projectile
void CLaser::Update(double dt)
{
	if (m_bStatus == false)
		return;

	// Update TimeLife of projectile. Set to inactive if too long
	m_fLifetime -= (float)dt;
	if (m_fLifetime < 0.0f)
	{
		SetStatus(false);
		SetIsDone(true);	// This method is to inform the EntityManager that it should remove this instance
		return;
	}

	// Update Position
	position.Set(	position.x + (float)(theDirection.x * dt * m_fSpeed),
					position.y + (float)(theDirection.y * dt * m_fSpeed),
					position.z + (float)(theDirection.z * dt * m_fSpeed));
}


// Render this projectile
void CLaser::Render(void)
{
	if (m_bStatus == false)
		return;

	if (m_fLifetime < 0.0f)
		return;

	MS& modelStack = GraphicsManager::GetInstance()->GetModelStack();
	modelStack.PushMatrix();
		// Reset the model stack
		modelStack.LoadIdentity();
		// We introduce a small offset to y position so that we can see the laser beam.
		modelStack.Translate(position.x, position.y-0.001f, position.z);
		modelStack.PushMatrix();
		modelStack.Rotate(180 / Math::PI * angle_x, 0.0f, 1.0f, 0.0f);
		modelStack.PushMatrix();
			modelStack.Rotate(180 / Math::PI * angle_y, 0.0f, 0.0f, 1.0f);
			modelStack.PushMatrix();
					modelStack.Scale(scale.x * m_fLength, scale.y, scale.z);
					glLineWidth(10.0f);
					RenderHelper::RenderMesh(modelMesh);
					glLineWidth(1.0f);
				modelStack.PopMatrix();
			modelStack.PopMatrix();
		modelStack.PopMatrix();
	modelStack.PopMatrix();
}

// Create a projectile and add it into EntityManager
CLaser* Create::Laser(const std::string& _meshName, 
								const Vector3& _position, 
								const Vector3& _direction, 
								const float m_fLength, 
								const float m_fLifetime, 
								const float m_fSpeed,
								CPlayerInfo* _source)
{
	Mesh* modelMesh = MeshBuilder::GetInstance()->GetMesh(_meshName);
	if (modelMesh == nullptr)
		return nullptr;

	CLaser* result = new CLaser(modelMesh);
	result->Set(_position, _direction, m_fLifetime, m_fSpeed);
	result->SetLength(m_fLength);
	result->SetStatus(true);
	result->SetCollider(true);
	result->SetSource(_source);
	// Calculate the angles of this laser with respect to the 3 axes
	result->CalculateAngles();
	EntityManager::GetInstance()->AddEntity(result, EntityManager::PROJECTILE);

	return result;
}