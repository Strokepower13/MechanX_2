#pragma once

class GameTimer
{
public:
	GameTimer();

	float totalTime() const;
	float deltaTime() const;

	void reset();
	void start();
	void stop();
	void tick();

private:
	double p_secondsPerCount;
	double p_deltaTime;

	__int64 p_baseTime;
	__int64 p_pausedTime;
	__int64 p_stopTime;
	__int64 p_prevTime;
	__int64 p_currTime;

	bool p_stopped;
};

