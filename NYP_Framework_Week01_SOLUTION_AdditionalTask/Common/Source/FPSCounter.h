#pragma once

#include "SingletonTemplate.h"

class CFPSCounter : public Singleton<CFPSCounter>
{
	friend Singleton<CFPSCounter>;
protected:
	CFPSCounter();

public:
	virtual ~CFPSCounter();

	// Initialise the class instance
	void Init(void);

	// Update the class instance
	void Update(const double deltaTime);

	// Get the current frame rate
	double GetFrameRate(void);

	// Get the current frame time
	double GetFrameTime(void) const;

	// Print Self
	void PrintSelf(void);

protected:
	// Count the elapsed time since the last reset
	double dElapsedTime;
	// Count the number of frames for the current second
	int nFrames;
	// The frame count for the last 1 second
	int iFrameRate;
	// Count the elapsed time since the last reset
	double dFrameTime;
	// Boolean flag to indicate if the average frame rate was calculated
	bool bAverageFrameRate;
};
