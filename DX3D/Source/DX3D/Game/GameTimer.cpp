#include <DX3D/Game/GameTimer.h>
#include <Windows.h>

GameTimer::GameTimer() :p_secondsPerCount(0.0), p_deltaTime(0.0), p_baseTime(0), p_pausedTime(0), p_stopTime(0), p_prevTime(0), p_currTime(0), p_stopped(false)
{
	__int64 countsPerSec = 0;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
	p_secondsPerCount = 1.0 / (double)countsPerSec;
}

float GameTimer::totalTime() const
{
	if (p_stopped)
		return(float)(((p_stopTime - p_pausedTime) - p_baseTime) * p_secondsPerCount);
	else
		return (float)(((p_currTime - p_pausedTime) - p_baseTime) * p_secondsPerCount);
}

float GameTimer::deltaTime() const
{
	return (float)p_deltaTime;
}

void GameTimer::reset()
{
	__int64 currTime = 0;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

	p_baseTime = currTime;
	p_prevTime = currTime;
	p_stopTime = 0;
	p_stopped = false;
}

void GameTimer::start()
{
	__int64 startTime = 0;
	QueryPerformanceCounter((LARGE_INTEGER*)&startTime);

	if (p_stopped)
	{
		p_pausedTime += (startTime - p_stopTime);
		p_prevTime = startTime;
		p_stopTime = 0;
		p_stopped = false;
	}
}

void GameTimer::stop()
{
	if (!p_stopped)
	{
		__int64 currTime = 0;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

		p_stopTime = currTime;
		p_stopped = true;
	}
}

void GameTimer::tick()
{
	if (p_stopped)
	{
		p_deltaTime = 0.0;
		return;
	}

	__int64 currTime = 0;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
	p_currTime = currTime;

	p_deltaTime = (p_currTime - p_prevTime) * p_secondsPerCount;

	p_prevTime = p_currTime;

	if (p_deltaTime < 0.0)
		p_deltaTime = 0.0;
}
