#include "PlayerInfo.h"
#include <iostream>

#include "MouseController.h"
#include "KeyboardController.h"
#include "Mtx44.h"
#include "../Projectile/Projectile.h"
#include "../WeaponInfo/Pistol.h"
#include "../WeaponInfo/RPG.h"
#include "../Minimap/Minimap.h"

// Allocating and initializing CPlayerInfo's static data member.  
// The pointer is allocated but not the object's constructor.
CPlayerInfo *CPlayerInfo::s_instance = 0;

CPlayerInfo::CPlayerInfo(void)
	: m_dSpeed(40.0)
	, m_dAcceleration(10.0)
	, m_bJumpUpwards(false)
	, m_dJumpSpeed(30.0)
	, m_dJumpAcceleration(-10.0)
	, m_bFallDownwards(false)
	, m_dFallSpeed(0.0)
	, m_dFallAcceleration(-10.0)
	, m_dElapsedTime(0.0)
	, attachedCamera(NULL)
	, m_pTerrain(NULL)
	, primaryWeapon(NULL)
	, secondaryWeapon(NULL)
	, theCurrentPosture(STAND)
	, weaponManager(NULL)
	, m_iCurrentWeapon(0)
	, m_fCameraSwayAngle(0.0f)
	, m_fCameraSwayDeltaAngle(0.1f)
	, m_fCameraSwayAngle_LeftLimit(-0.3f)
	, m_fCameraSwayAngle_RightLimit(0.3f)
	, m_bCameraSwayDirection(false)
{
}

CPlayerInfo::~CPlayerInfo(void)
{
	if (weaponManager)
	{
		for (int i = 0; i < m_iNumOfWeapon; i++)
		{
			delete weaponManager[i];
		}
		delete [] weaponManager;
		weaponManager = NULL;
	}
	if (secondaryWeapon)
	{
		delete secondaryWeapon;
		secondaryWeapon = NULL;
	}
	if (primaryWeapon)
	{
		delete primaryWeapon;
		primaryWeapon = NULL;
	}
	m_pTerrain = NULL;
}

// Initialise this class instance
void CPlayerInfo::Init(void)
{
	// Set the default values
	defaultPosition.Set(0,0,10);
	defaultTarget.Set(0,0,0);
	defaultUp.Set(0,1,0);

	// Set the current values
	position.Set(0, 0, 10);
	target.Set(0, 0, 0);
	up.Set(0, 1, 0);

	// Set Boundary
	maxBoundary.Set(1,1,1);
	minBoundary.Set(-1, -1, -1);

	// Set the pistol as the primary weapon
	primaryWeapon = new CPistol();
	primaryWeapon->Init();

	secondaryWeapon = new CRPG();
	secondaryWeapon->Init();

	weaponManager = new CWeaponInfo*[m_iNumOfWeapon];
	for (int i = 0; i < m_iNumOfWeapon; i++)
	{
		weaponManager[i] = new CPistol();
		weaponManager[i]->Init();
	}

	m_fCameraSwayAngle = 0.0f;
	m_fCameraSwayDeltaAngle = 1.0f;
	m_fCameraSwayAngle_LeftLimit = -5.0f;
	m_fCameraSwayAngle_RightLimit = 5.0f;
	m_bCameraSwayDirection = false;
}

// Returns true if the player is on ground
bool CPlayerInfo::isOnGround(void)
{
	if (m_bJumpUpwards == false && m_bFallDownwards == false)
		return true;

	return false;
}

// Returns true if the player is jumping upwards
bool CPlayerInfo::isJumpUpwards(void)
{
	if (m_bJumpUpwards == true && m_bFallDownwards == false)
		return true;

	return false;
}

// Returns true if the player is on freefall
bool CPlayerInfo::isFreeFall(void)
{
	if (m_bJumpUpwards == false && m_bFallDownwards == true)
		return true;

	return false;
}

// Set the player's status to free fall mode
void CPlayerInfo::SetOnFreeFall(bool isOnFreeFall)
{
	if (isOnFreeFall == true)
	{
		m_bJumpUpwards = false;
		m_bFallDownwards = true;
		m_dFallSpeed = 0.0;
	}
}

// Set the player to jumping upwards
void CPlayerInfo::SetToJumpUpwards(bool isOnJumpUpwards)
{
	if (isOnJumpUpwards == true)
	{
		m_bJumpUpwards = true;
		m_bFallDownwards = false;
		m_dJumpSpeed = 10.0;
	}
}

// Set position
void CPlayerInfo::SetPos(const Vector3& pos)
{
	position = pos;
}

// Set target
void CPlayerInfo::SetTarget(const Vector3& target)
{
	this->target = target;
}

// Set position
void CPlayerInfo::SetUp(const Vector3& up)
{
	this->up = up;
}

// Set m_dJumpAcceleration of the player
void CPlayerInfo::SetJumpAcceleration(const double m_dJumpAcceleration)
{
	this->m_dJumpAcceleration = m_dJumpAcceleration;
}

// Set Fall Acceleration of the player
void CPlayerInfo::SetFallAcceleration(const double m_dFallAcceleration)
{
	this->m_dFallAcceleration = m_dFallAcceleration;
}

// Set the boundary for the player info
void CPlayerInfo::SetBoundary(Vector3 max, Vector3 min)
{
	maxBoundary = max;
	minBoundary = min;
}

// Set the terrain for the player info
void CPlayerInfo::SetTerrain(GroundEntity* m_pTerrain)
{
	if (m_pTerrain != NULL)
	{
		this->m_pTerrain = m_pTerrain;

		SetBoundary(this->m_pTerrain->GetMaxBoundary(), this->m_pTerrain->GetMinBoundary());
	}
}

// Stop the player's movement
void CPlayerInfo::StopVerticalMovement(void)
{
	m_bJumpUpwards = false;
	m_bFallDownwards = false;
}

// Reset this player instance to default
void CPlayerInfo::Reset(void)
{
	// Set the current values to default values
	position = defaultPosition;
	target = defaultTarget;
	up = defaultUp;

	// Stop vertical movement too
	StopVerticalMovement();
}

// Get position x of the player
Vector3 CPlayerInfo::GetPos(void) const
{
	return position;
}

// Get target
Vector3 CPlayerInfo::GetTarget(void) const
{
	return target;
}
// Get Up
Vector3 CPlayerInfo::GetUp(void) const
{
	return up;
}

// Get m_dJumpAcceleration of the player
double CPlayerInfo::GetJumpAcceleration(void) const
{
	return m_dJumpAcceleration;
}

// Update Jump Upwards
void CPlayerInfo::UpdateJumpUpwards(double dt)
{
	if (m_bJumpUpwards == false)
		return;

	// Update the jump's elapsed time
	m_dElapsedTime += dt;

	// Update position and target y values
	// Use SUVAT equation to update the change in position and target
	// s = u * t + 0.5 * a * t ^ 2
	position.y += (float)(m_dJumpSpeed * m_dElapsedTime + 
						  0.5 * m_dJumpAcceleration * m_dElapsedTime * m_dElapsedTime);
	target.y += (float)(m_dJumpSpeed * m_dElapsedTime + 
						0.5 * m_dJumpAcceleration * m_dElapsedTime * m_dElapsedTime);
	// Use this equation to calculate final velocity, v
	// SUVAT: v = u + a * t; v is m_dJumpSpeed AFTER updating using SUVAT where u is 
	// the initial speed and is equal to m_dJumpSpeed
	m_dJumpSpeed = m_dJumpSpeed + m_dJumpAcceleration * m_dElapsedTime;
	// Check if the jump speed is less than zero, then it should be falling
	if (m_dJumpSpeed < 0.0)
	{
		m_dJumpSpeed = 0.0;
		m_bJumpUpwards = false;
		m_dFallSpeed = 0.0;
		m_bFallDownwards = true;
		m_dElapsedTime = 0.0;
	}
}

// Update FreeFall
void CPlayerInfo::UpdateFreeFall(double dt)
{
	if (m_bFallDownwards == false)
		return;

	// Update the jump's elapsed time
	m_dElapsedTime += dt;

	// Update position and target y values.
	// Use SUVAT equation to update the change in position and target
	// s = u * t + 0.5 * a * t ^ 2
	position.y += (float)(m_dFallSpeed * m_dElapsedTime + 
						  0.5 * m_dJumpAcceleration * m_dElapsedTime * m_dElapsedTime);
	target.y += (float)(m_dFallSpeed * m_dElapsedTime + 
						0.5 * m_dJumpAcceleration * m_dElapsedTime * m_dElapsedTime);
	// Use this equation to calculate final velocity, v
	// SUVAT: v = u + a * t;
	// v is m_dJumpSpeed AFTER updating using SUVAT where u is the initial speed and is equal to m_dJumpSpeed
	m_dFallSpeed = m_dFallSpeed + m_dFallAcceleration * m_dElapsedTime;
	// Check if the jump speed is below terrain, then it should be reset to terrain height
	if (position.y < m_pTerrain->GetTerrainHeight(position))
	{
		Vector3 viewDirection = target - position;
		position.y = m_pTerrain->GetTerrainHeight(position);
		target = position + viewDirection;
		m_dFallSpeed = 0.0;
		m_bFallDownwards = false;
		m_dElapsedTime = 0.0;
	}
}

/********************************************************************************
 Hero Update
 ********************************************************************************/
void CPlayerInfo::Update(double dt)
{
	//double mouse_diff_x, mouse_diff_y;
	//MouseController::GetInstance()->GetMouseDelta(mouse_diff_x, mouse_diff_y);

	//double camera_yaw = mouse_diff_x * 0.0174555555555556;		// 3.142 / 180.0
	//double camera_pitch = mouse_diff_y * 0.0174555555555556;	// 3.142 / 180.0

	// Check if there is a need to change posture
	if (KeyboardController::GetInstance()->IsKeyReleased('Z'))
	{
		theCurrentPosture = (CURRENT_POSTURE)(theCurrentPosture + 1);
		if (theCurrentPosture == NUM_POSTURE)
			theCurrentPosture = STAND;

		Vector3 viewDirection = target - position;
		switch (theCurrentPosture)
		{
		case STAND:
			position = m_pTerrain->GetTerrainHeight(Vector3(position.x, 0.0f, position.z));
			target = position + viewDirection;
			break;
		case CROUCH:
			position = m_pTerrain->GetTerrainHeight(Vector3(position.x, 0.0f, position.z));
			position.y -= 5.0f;
			target = position + viewDirection;
			break;
		case PRONE:
			position = m_pTerrain->GetTerrainHeight(Vector3(position.x, 0.0f, position.z));
			position.y -= 8.0f;
			target = position + viewDirection;
			break;
		default:
			break;
		}
	}

	//// Update the position if the WASD buttons were activated
	//if (KeyboardController::GetInstance()->IsKeyDown('W') ||
	//	KeyboardController::GetInstance()->IsKeyDown('A') ||
	//	KeyboardController::GetInstance()->IsKeyDown('S') ||
	//	KeyboardController::GetInstance()->IsKeyDown('D'))
	//{
	//	Vector3 viewVector = target - position;
	//	Vector3 rightUV;
	//	if (KeyboardController::GetInstance()->IsKeyDown('W'))
	//	{
	//		if ((KeyboardController::GetInstance()->IsKeyDown('W')) && (KeyboardController::GetInstance()->IsKeyDown(VK_SHIFT)))
	//			position += viewVector.Normalized() * (float)m_dSpeed * 2.0f * (float)dt;
	//		else
	//			position += viewVector.Normalized() * (float)m_dSpeed * (float)dt;
	//	}
	//	else if (KeyboardController::GetInstance()->IsKeyDown('S'))
	//	{
	//		position -= viewVector.Normalized() * (float)m_dSpeed * (float)dt;
	//	}
	//	if (KeyboardController::GetInstance()->IsKeyDown('A'))
	//	{
	//		rightUV = (viewVector.Normalized()).Cross(up);
	//		rightUV.y = 0;
	//		rightUV.Normalize();
	//		position -= rightUV * (float)m_dSpeed * (float)dt;
	//	}
	//	else if (KeyboardController::GetInstance()->IsKeyDown('D'))
	//	{
	//		rightUV = (viewVector.Normalized()).Cross(up);
	//		rightUV.y = 0;
	//		rightUV.Normalize();
	//		position += rightUV * (float)m_dSpeed * (float)dt;
	//	}
	//	 Constrain the position
	//	Constrain();
	//	// Update the target
	//	target = position + viewVector;
	//}

	//// Rotate the view direction
	//if (KeyboardController::GetInstance()->IsKeyDown(VK_LEFT) ||
	//	KeyboardController::GetInstance()->IsKeyDown(VK_RIGHT) ||
	//	KeyboardController::GetInstance()->IsKeyDown(VK_UP) ||
	//	KeyboardController::GetInstance()->IsKeyDown(VK_DOWN))
	//{
	//	Vector3 viewUV = (target - position).Normalized();
	//	Vector3 rightUV;
	//	if (KeyboardController::GetInstance()->IsKeyDown(VK_LEFT))
	//	{
	//		float yaw = (float)m_dSpeed * (float)dt;
	//		Mtx44 rotation;
	//		rotation.SetToRotation(yaw, 0, 1, 0);
	//		viewUV = rotation * viewUV;
	//		target = position + viewUV;
	//		rightUV = viewUV.Cross(up);
	//		rightUV.y = 0;
	//		rightUV.Normalize();
	//		up = rightUV.Cross(viewUV).Normalized();
	//	}
	//	else if (KeyboardController::GetInstance()->IsKeyDown(VK_RIGHT))
	//	{
	//		float yaw = (float)(-m_dSpeed * (float)dt);
	//		Mtx44 rotation;
	//		rotation.SetToRotation(yaw, 0, 1, 0);
	//		viewUV = rotation * viewUV;
	//		target = position + viewUV;
	//		rightUV = viewUV.Cross(up);
	//		rightUV.y = 0;
	//		rightUV.Normalize();
	//		up = rightUV.Cross(viewUV).Normalized();
	//	}
	//	if (KeyboardController::GetInstance()->IsKeyDown(VK_UP))
	//	{
	//		float pitch = (float)(m_dSpeed * (float)dt);
	//		rightUV = viewUV.Cross(up);
	//		rightUV.y = 0;
	//		rightUV.Normalize();
	//		up = rightUV.Cross(viewUV).Normalized();
	//		Mtx44 rotation;
	//		rotation.SetToRotation(pitch, rightUV.x, rightUV.y, rightUV.z);
	//		viewUV = rotation * viewUV;
	//		target = position + viewUV;
	//	}
	//	else if (KeyboardController::GetInstance()->IsKeyDown(VK_DOWN))
	//	{
	//		float pitch = (float)(-m_dSpeed * (float)dt);
	//		rightUV = viewUV.Cross(up);
	//		rightUV.y = 0;
	//		rightUV.Normalize();
	//		up = rightUV.Cross(viewUV).Normalized();
	//		Mtx44 rotation;
	//		rotation.SetToRotation(pitch, rightUV.x, rightUV.y, rightUV.z);
	//		viewUV = rotation * viewUV;
	//		target = position + viewUV;
	//	}
	//}

	////Update the camera direction based on mouse move
	//{
	//	Vector3 viewUV = (target - position).Normalized();
	//	Vector3 rightUV;

	//	if (camera_yaw != 0.0)
	//	{
	//		float yaw = (float)(-m_dSpeed * camera_yaw * (float)dt);
	//		Mtx44 rotation;
	//		rotation.SetToRotation(yaw, 0, 1, 0);
	//		viewUV = rotation * viewUV;
	//		target = position + viewUV;
	//		rightUV = viewUV.Cross(up);
	//		rightUV.y = 0;
	//		rightUV.Normalize();
	//		up = rightUV.Cross(viewUV).Normalized();
	//		
	//		// Update the minimap rotation angle
	//		CMinimap::GetInstance()->SetAngle(atan2(viewUV.z, viewUV.x) * 57.2883513685549146);
	//	}
	//	{
	//		float pitch = (float)(-m_dSpeed * camera_pitch * (float)dt);
	//		rightUV = viewUV.Cross(up);
	//		rightUV.y = 0;
	//		rightUV.Normalize();
	//		up = rightUV.Cross(viewUV).Normalized();
	//		Mtx44 rotation;
	//		rotation.SetToRotation(pitch, rightUV.x, rightUV.y, rightUV.z);
	//		viewUV = rotation * viewUV;
	//		target = position + viewUV;
	//	}
	//}

	//// If the user presses SPACEBAR, then make him jump
	//if (KeyboardController::GetInstance()->IsKeyDown(VK_SPACE) &&
	//	position.y == m_pTerrain->GetTerrainHeight(position))
	//{
	//	SetToJumpUpwards(true);
	//}

	//// Update the weapons
	//if (KeyboardController::GetInstance()->IsKeyReleased('R'))
	//{
	//	//if (primaryWeapon)
	//	//{
	//	//	primaryWeapon->Reload();
	//	//	//primaryWeapon->PrintSelf();
	//	//}
	//	if (weaponManager[m_iCurrentWeapon])
	//		weaponManager[m_iCurrentWeapon]->Reload();
	//}
	if (primaryWeapon)
		primaryWeapon->Update(dt);
	if (secondaryWeapon)
		secondaryWeapon->Update(dt);
	if (weaponManager[m_iCurrentWeapon])
		weaponManager[m_iCurrentWeapon]->Update(dt);

	//if (MouseController::GetInstance()->GetMouseScrollStatus(MouseController::SCROLL_TYPE_YOFFSET) != m_iCurrentWeapon)
	//{
	//	if ((MouseController::GetInstance()->GetMouseScrollStatus(MouseController::SCROLL_TYPE_YOFFSET) >= 0) &&
	//		(MouseController::GetInstance()->GetMouseScrollStatus(MouseController::SCROLL_TYPE_YOFFSET) < m_iNumOfWeapon))
	//	{
	//		m_iCurrentWeapon = MouseController::GetInstance()->GetMouseScrollStatus(MouseController::SCROLL_TYPE_YOFFSET);
	//	}
	//}

	//// if Mouse Buttons were activated, then act on them
	//if (MouseController::GetInstance()->IsButtonPressed(MouseController::LMB))
	//{
	//	//if (primaryWeapon)
	//	//	primaryWeapon->Discharge(position, target, this);

	//	if (weaponManager[m_iCurrentWeapon])
	//		weaponManager[m_iCurrentWeapon]->Discharge(position, target, this);
	//}
	//else if (MouseController::GetInstance()->IsButtonPressed(MouseController::RMB))
	//{

	//}

	//// If the user presses R key, then reset the view to default values
	//if (KeyboardController::GetInstance()->IsKeyDown('P'))
	//{
	//	Reset();
	//}
	//else
	{
		// Constrain the position
		Constrain();
		UpdateJumpUpwards(dt);
		UpdateFreeFall(dt);
	}

	// Do camera sway
	if (m_fCameraSwayAngle != 0.0f)
	{
		Mtx44 rotation;
		if (m_bCameraSwayDirection == false)
			rotation.SetToRotation(-m_fCameraSwayDeltaAngle, 0.0f, 0.0f, 1.0f);
		else if (m_bCameraSwayDirection == true)
			rotation.SetToRotation(m_fCameraSwayDeltaAngle, 0.0f, 0.0f, 1.0f);
		up = rotation * up;
	}

	// Update minimap rotation angle
	Vector3 viewUV = (target - position).Normalized();
	CMinimap::GetInstance()->SetAngle((int)(atan2(viewUV.z, viewUV.x) * 57.2883513685549146));

	// If a camera is attached to this playerInfo class, then update it
	if (attachedCamera)
	{
		attachedCamera->SetCameraPos(position);
		attachedCamera->SetCameraTarget(target);
		attachedCamera->SetCameraUp(up);
	}
}

// Detect and process front / back movement on the controller
bool CPlayerInfo::Move_FrontBack(const float deltaTime, const bool direction, const float speedMultiplier)
{
	// Add camera sway
	if (m_bCameraSwayDirection == false)
	{
		m_fCameraSwayAngle -= m_fCameraSwayDeltaAngle;
		if (m_fCameraSwayAngle < m_fCameraSwayAngle_LeftLimit * speedMultiplier)
			m_bCameraSwayDirection = !m_bCameraSwayDirection;
	}
	else
	{
		m_fCameraSwayAngle += m_fCameraSwayDeltaAngle;
		if (m_fCameraSwayAngle > m_fCameraSwayAngle_RightLimit * speedMultiplier)
			m_bCameraSwayDirection = !m_bCameraSwayDirection;
	}

	Vector3 viewVector = (target - position).Normalized();
	if (direction)
	{
		position += viewVector * (float)m_dSpeed * speedMultiplier * (float)deltaTime;
		//	 Constrain the position
		Constrain();
		// Update the target
		target = position + viewVector;

		return true;
	}
	else
	{
		position -= viewVector * (float)m_dSpeed * (float)deltaTime;
		//	 Constrain the position
		Constrain();
		// Update the target
		target = position + viewVector;
		return true;
	}

	return false;
}
// Detect and process left / right movement on the controller
bool CPlayerInfo::Move_LeftRight(const float deltaTime, const bool direction, const float speedMultiplier)
{
	Vector3 viewVector = target - position;
	Vector3 rightUV;
	if (direction)
	{
		rightUV = (viewVector.Normalized()).Cross(up);
		rightUV.y = 0;
		rightUV.Normalize();
		position -= rightUV * (float)m_dSpeed * deltaTime;
		// Update the target
		target = position + viewVector;
		return true;
	}
	else
	{
		rightUV = (viewVector.Normalized()).Cross(up);
		rightUV.y = 0;
		rightUV.Normalize();
		position += rightUV * (float)m_dSpeed * deltaTime;
		// Update the target
		target = position + viewVector;
		return true;
	}
	return false;
}

// Detect and process look up / down on the controller
bool CPlayerInfo::Look_UpDown(const float deltaTime, const bool direction, const float speedMultiplier)
{
	if (speedMultiplier == 0.0f)
		return false;

	Vector3 viewUV = (target - position).Normalized();
	Vector3 rightUV;

	float pitch = (float)(-m_dSpeed * speedMultiplier * (float)deltaTime);
	rightUV = viewUV.Cross(up);
	rightUV.y = 0;
	rightUV.Normalize();
	up = rightUV.Cross(viewUV).Normalized();
	Mtx44 rotation;
	rotation.SetToRotation(pitch, rightUV.x, rightUV.y, rightUV.z);
	viewUV = rotation * viewUV;
	target = position + viewUV;

	return true;
}
// Detect and process look left / right on the controller
bool CPlayerInfo::Look_LeftRight(const float deltaTime, const bool direction, const float speedMultiplier)
{
	if (speedMultiplier == 0.0f)
		return false;

	Vector3 viewUV = (target - position).Normalized();
	Vector3 rightUV;

	float yaw = (float)-m_dSpeed * speedMultiplier * (float)deltaTime;
	Mtx44 rotation;
	rotation.SetToRotation(yaw, 0, 1, 0);
	viewUV = rotation * viewUV;
	target = position + viewUV;
	rightUV = viewUV.Cross(up);
	rightUV.y = 0;
	rightUV.Normalize();
	up = rightUV.Cross(viewUV).Normalized();

	return true;
}

// Stop sway
bool CPlayerInfo::StopSway(const float deltaTime)
{
	m_bCameraSwayDirection = false;
	m_fCameraSwayAngle = 0.0f;
	up = Vector3(0.0f, 1.0f, 0.0f);
	return true;
}

// Reload current weapon
bool CPlayerInfo::ReloadWeapon(void)
{
	if (weaponManager[m_iCurrentWeapon])
	{
		weaponManager[m_iCurrentWeapon]->Reload();
		return true;
	}
	return false;
}

// Change current weapon
bool CPlayerInfo::ChangeWeapon(void)
{
	if (MouseController::GetInstance()->GetMouseScrollStatus(MouseController::SCROLL_TYPE_YOFFSET) != m_iCurrentWeapon)
	{
		if ((MouseController::GetInstance()->GetMouseScrollStatus(MouseController::SCROLL_TYPE_YOFFSET) >= 0) &&
			(MouseController::GetInstance()->GetMouseScrollStatus(MouseController::SCROLL_TYPE_YOFFSET) < m_iNumOfWeapon))
		{
			m_iCurrentWeapon = (int) MouseController::GetInstance()->GetMouseScrollStatus(MouseController::SCROLL_TYPE_YOFFSET);
		}
	}
	return true;
}

// Get Current Weapon
int CPlayerInfo::GetWeapon(void) const
{
	return m_iCurrentWeapon;
}

// Discharge Primary Weapon
bool CPlayerInfo::DischargePrimaryWeapon(const float deltaTime)
{
	//if (primaryWeapon)
	//	primaryWeapon->Discharge(position, target, this);

	if (weaponManager[m_iCurrentWeapon])
	{
		weaponManager[m_iCurrentWeapon]->Discharge(position, target, this);
		return true;
	}

	return false;
}

// Discharge Secondary Weapon
bool CPlayerInfo::DischargeSecondaryWeapon(const float deltaTime)
{
	if (secondaryWeapon)
	{
		secondaryWeapon->Discharge(position, target, this);
		return true;
	}

	return false;
}

// Constrain the position within the borders
void CPlayerInfo::Constrain(void)
{
	// Constrain player within the boundary
	if (position.x > maxBoundary.x - 1.0f)
		position.x = maxBoundary.x - 1.0f;
	if (position.y > maxBoundary.y - 1.0f)
	{
		position.y = maxBoundary.y - 1.0f;
		m_dJumpSpeed = 0.0;
		m_bJumpUpwards = false;
		m_dFallSpeed = 0.0;
		m_bFallDownwards = true;
		m_dElapsedTime = 0.0;
	}
	if (position.z > maxBoundary.z - 1.0f)
		position.z = maxBoundary.z - 1.0f;
	if (position.x < minBoundary.x + 1.0f)
		position.x = minBoundary.x + 1.0f;
	if (position.y < minBoundary.y + 1.0f)
		position.y = minBoundary.y + 1.0f;
	if (position.z < minBoundary.z + 1.0f)
		position.z = minBoundary.z + 1.0f;

	// if the player is not jumping nor falling, then adjust his y position
	if ((m_bJumpUpwards == false) && (m_bFallDownwards == false))
	{
		// if the y position is not equal to terrain height at that position, 
		// then update y position to the terrain height
		if (position.y != m_pTerrain->GetTerrainHeight(position))
			position.y = m_pTerrain->GetTerrainHeight(position);
	}
}

void CPlayerInfo::AttachCamera(FPSCamera* _cameraPtr)
{
	attachedCamera = _cameraPtr;
}

void CPlayerInfo::DetachCamera()
{
	attachedCamera = nullptr;
}
