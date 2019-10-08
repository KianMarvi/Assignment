#pragma once

#include "Controller.h"

class CKeyboard :
	public CController
{
public:
	CKeyboard();
	virtual ~CKeyboard();
	// Create this controller
	virtual bool Create(CPlayerInfo* thePlayerInfo = NULL);
	// Read from the controller
	virtual int Read(const float deltaTime);
};

