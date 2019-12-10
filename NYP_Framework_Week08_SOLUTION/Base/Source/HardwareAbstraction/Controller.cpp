#include "Controller.h"
#include <iostream>
using namespace std;

const bool _CONTROLLER_DEBUG = false;

CController::CController()
	: thePlayerInfo(NULL)
{
}


CController::~CController()
{
	// We just set thePlayerInfo to NULL without deleting. SceneText will delete this.
	if (thePlayerInfo)
		thePlayerInfo = NULL;
}


// Create this controller
bool CController::Create(CPlayerInfo* thePlayerInfo)
{
	if (_CONTROLLER_DEBUG)
		cout << "CController::Create()" << endl;
	this->thePlayerInfo = thePlayerInfo;
	return false;
}

// Read from the controller
int CController::Read(const float deltaTime)
{
	if (_CONTROLLER_DEBUG)
		cout << "CController::Read()" << endl;
	return 0;
}

// Detect and process front / back movement on the controller
bool CController::Move_FrontBack(const float deltaTime, const bool direction, const float speedMultiplier)
{
	if (_CONTROLLER_DEBUG)
		cout << "CController::Move_FrontBack()" << endl;
	if (thePlayerInfo)
	{
		thePlayerInfo->Move_FrontBack(deltaTime, direction, speedMultiplier);
	}
	return false;
}

// Detect and process left / right movement on the controller
bool CController::Move_LeftRight(const float deltaTime, const bool direction)
{
	if (_CONTROLLER_DEBUG)
		cout << "CController::Move_LeftRight()" << endl;
	if (thePlayerInfo)
	{
		thePlayerInfo->Move_LeftRight(deltaTime, direction);
	}
	return false;
}

// Detect and process look up / down on the controller
bool CController::Look_UpDown(const float deltaTime, const bool direction, const float speedMultiplier)
{
	if (_CONTROLLER_DEBUG)
		cout << "CController::Look_UpDown()" << endl;
	if (thePlayerInfo)
	{
		thePlayerInfo->Look_UpDown(deltaTime, direction, speedMultiplier);
	}
	return false;
}

// Detect and process look left / right on the controller
bool CController::Look_LeftRight(const float deltaTime, const bool direction, const float speedMultiplier)
{
	if (_CONTROLLER_DEBUG)
		cout << "CController::Look_LeftRight()" << endl;
	if (thePlayerInfo)
	{
		thePlayerInfo->Look_LeftRight(deltaTime, direction, speedMultiplier);
	}
	return false;
}

// Stop sway
bool CController::StopSway(const float deltaTime)
{
	if (_CONTROLLER_DEBUG)
		cout << "CController::StopSway()" << endl;
	if (thePlayerInfo)
	{
		thePlayerInfo->StopSway(deltaTime);
	}
	return false;
}

// Jump
bool CController::Jump(const float deltaTime)
{
	if (_CONTROLLER_DEBUG)
		cout << "CController::Jump()" << endl;
	if (thePlayerInfo)
	{
		thePlayerInfo->SetToJumpUpwards(true);
	}
	return false;
}

// Reload current weapon
bool CController::Reload(const float deltaTime)
{
	if (_CONTROLLER_DEBUG)
		cout << "CController::Reload()" << endl;
	if (thePlayerInfo)
	{
		thePlayerInfo->ReloadWeapon();
	}
	return false;
}

// Change current weapon (for primary only)
bool CController::Change(const float deltaTime)
{
	if (_CONTROLLER_DEBUG)
		cout << "CController::Change()" << endl;
	if (thePlayerInfo)
	{
		thePlayerInfo->ChangeWeapon();
	}
	return false;
}

// Fire primary weapon
bool CController::FirePrimary(const float deltaTime)
{
	if (_CONTROLLER_DEBUG)
		cout << "CController::FirePrimary()" << endl;
	if (thePlayerInfo)
	{
		thePlayerInfo->DischargePrimaryWeapon(deltaTime);
	}
	return false;
}

// Fire secondary weapon
bool CController::FireSecondary(const float deltaTime)
{
	if (_CONTROLLER_DEBUG)
		cout << "CController::FireSecondary()" << endl;
	if (thePlayerInfo)
	{
		thePlayerInfo->DischargeSecondaryWeapon(deltaTime);
	}
	return false;
}

// Reset the PlayerInfo
bool CController::Reset(void)
{
	if (_CONTROLLER_DEBUG)
		cout << "CController::Reset()" << endl;
	if (thePlayerInfo)
	{
		thePlayerInfo->Reset();
	}
	return false;
}
