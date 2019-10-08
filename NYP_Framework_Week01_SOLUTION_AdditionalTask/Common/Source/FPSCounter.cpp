#include "FPSCounter.h"
#include <iostream>
#include <queue> 
using namespace std;

// Queue containing the frame rates
queue<int> frameRateQueue;

CFPSCounter::CFPSCounter()
	: dElapsedTime(0.0)
	, nFrames(0)
	, iFrameRate(0)
	, dFrameTime(0.0)
	, bAverageFrameRate(false)
{
	Init();
}


CFPSCounter::~CFPSCounter()
{
	// Empty the queue
	while (!frameRateQueue.empty())
	{
		frameRateQueue.pop();
	}
}


// Initialise the class instance
void CFPSCounter::Init(void)
{
	dElapsedTime = 0.0;
	nFrames = 0;
	iFrameRate = 0;
	dFrameTime = 60;
}

// Update the class instance
void CFPSCounter::Update(const double deltaTime)
{
	// Update elapsed time
	dElapsedTime += deltaTime;

	// Update the frame count
	nFrames++;

	if (dElapsedTime >= 1.0){ // If last update was more than 1 sec ago...
		// Calculate the current frame rate
		dFrameTime = 1000.0 / double(nFrames);

		// Update the frame count for the last 1 second
		//iFrameRate = nFrames;

		// Push into Queue 
		frameRateQueue.push(nFrames);
		bAverageFrameRate = false;
		if (frameRateQueue.size() > 5)
		{
			frameRateQueue.pop();
		}

		// Reset timer and update the lastTime
		nFrames = 0;
		dElapsedTime = 0.0;
	}
}

// Get the current frame rate
double CFPSCounter::GetFrameRate(void)
{
	cout << "CFPSCounter::GetFrameRate...";
	if ((bAverageFrameRate==false) && (!frameRateQueue.empty()))
	{
		// Reset iFrameRate to 0
		iFrameRate = 0;

		// Calculate the average frame rate
		queue<int> tempFrameRateQueue = frameRateQueue;
		while (!tempFrameRateQueue.empty())
		{
			iFrameRate += tempFrameRateQueue.front();
			tempFrameRateQueue.pop();
		}
		iFrameRate = iFrameRate / frameRateQueue.size();
		bAverageFrameRate = true;
		cout << "ReCalculated";

	}
	cout << "...CFPSCounter::GetFrameRate" << endl;

	return iFrameRate;
}

// Get the current frame time
double CFPSCounter::GetFrameTime(void) const
{
	return dFrameTime;
}

void CFPSCounter::PrintSelf(void)
{
	cout << "CFPSCounter::PrintSelf() :" << endl;
	cout << "==========================" << endl;
	cout << "Frame rates : " << endl;
	int iCounter = 1;
	int tempValue = -1;
	// Printing content of queue 
	queue<int> tempFrameRateQueue = frameRateQueue;
	while (!tempFrameRateQueue.empty())
	{
		tempValue = tempFrameRateQueue.front();
		cout << iCounter++ << " : " << tempValue << "." << endl;
		tempFrameRateQueue.pop();
	}
	cout << "==========================" << endl;
}