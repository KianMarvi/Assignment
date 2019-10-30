#include "Keyboard.h"
#include <iostream>
using namespace std;

#include "KeyboardController.h"
#include "../PlayerInfo/PlayerInfo.h"

const bool _CONTROLLER_KEYBOARD_DEBUG = false;

CKeyboard::CKeyboard()
{
}


CKeyboard::~CKeyboard()
{
}

// Create this controller
bool CKeyboard::Create(CPlayerInfo* thePlayerInfo)
{
	CController::Create(thePlayerInfo);
	if (_CONTROLLER_KEYBOARD_DEBUG)
		cout << "CKeyboard::Create()" << endl;
	return false;
}


// Read from the controller
int CKeyboard::Read(const float deltaTime)
{
	CController::Read(deltaTime);
	if (_CONTROLLER_KEYBOARD_DEBUG)
		cout << "CKeyboard::Read()" << endl;
	
	// Process the keys for customisation
	if (KeyboardController::GetInstance()->IsKeyDown('W'))
		Move_FrontBack(deltaTime, true);
	else if ((KeyboardController::GetInstance()->IsKeyDown('W')) && (KeyboardController::GetInstance()->IsKeyDown(VK_SHIFT)))
		Move_FrontBack(deltaTime, true, 2.0f);
	else if (KeyboardController::GetInstance()->IsKeyReleased('W'))
		StopSway(deltaTime);
	if (KeyboardController::GetInstance()->IsKeyDown('S'))
		Move_FrontBack(deltaTime, false);
	else if (KeyboardController::GetInstance()->IsKeyReleased('S'))
		StopSway(deltaTime);

	if (KeyboardController::GetInstance()->IsKeyDown('A'))
		Move_LeftRight(deltaTime, true);
	if (KeyboardController::GetInstance()->IsKeyDown('D'))
		Move_LeftRight(deltaTime, false);

	if (KeyboardController::GetInstance()->IsKeyDown(VK_UP))
		Look_UpDown(deltaTime, true);
	if (KeyboardController::GetInstance()->IsKeyDown(VK_DOWN))
		Look_UpDown(deltaTime, false);
	if (KeyboardController::GetInstance()->IsKeyDown(VK_LEFT))
		Look_LeftRight(deltaTime, true);
	if (KeyboardController::GetInstance()->IsKeyDown(VK_RIGHT))
		Look_LeftRight(deltaTime, false);

	// If the user presses SPACEBAR, then make him jump
	if (KeyboardController::GetInstance()->IsKeyDown(VK_SPACE) &&
		thePlayerInfo->isOnGround())
		Jump(true);

	// Update the weapons
	if (KeyboardController::GetInstance()->IsKeyReleased('R'))
		Reload(deltaTime);

	// If the user presses R key, then reset the view to default values
	if (KeyboardController::GetInstance()->IsKeyDown('P'))
		Reset();

	return 0;
}
