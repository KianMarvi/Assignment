#include "Mouse.h"
#include <iostream>
using namespace std;

#include "MouseController.h"
#include "../PlayerInfo/PlayerInfo.h"

const bool _CONTROLLER_MOUSE_DEBUG = false;

CMouse::CMouse()
{
}


CMouse::~CMouse()
{
}

// Create this controller
bool CMouse::Create(CPlayerInfo* thePlayerInfo)
{
	CController::Create(thePlayerInfo);
	if (_CONTROLLER_MOUSE_DEBUG)
		cout << "CMouse::Create()" << endl;

	return false;
}


// Read from the controller
int CMouse::Read(const float deltaTime)
{
	CController::Read(deltaTime);
	if (_CONTROLLER_MOUSE_DEBUG)
		cout << "CMouse::Read()" << endl;

	double mouse_diff_x, mouse_diff_y;
	MouseController::GetInstance()->GetMouseDelta(mouse_diff_x, mouse_diff_y);

	// Process the keys for customisation
	if (mouse_diff_x != 0.0)
		Look_LeftRight(deltaTime, true, (float)mouse_diff_x);
	if (mouse_diff_y != 0.0)
		Look_UpDown(deltaTime, true, (float)mouse_diff_y);

	if (MouseController::GetInstance()->GetMouseScrollStatus(MouseController::SCROLL_TYPE_YOFFSET) != thePlayerInfo->GetWeapon())
		Change(deltaTime);

	// if Mouse Buttons were activated, then act on them
	if (MouseController::GetInstance()->IsButtonPressed(MouseController::LMB))
		FirePrimary(deltaTime);

	else if (MouseController::GetInstance()->IsButtonPressed(MouseController::RMB))
		FireSecondary(deltaTime);

	return 0;
}
