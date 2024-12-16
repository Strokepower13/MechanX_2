#pragma once
#include <DX3D/Prerequisites.h>
#include <Windows.h>

class Game
{
public:
	Game(HINSTANCE hInstance);
	virtual ~Game();

	void run();
	void quit();

	Display* getDisplay();
	GameTimer* getTimer();
	GraphicsEngine* getGraphicsEngine();
	InputSystem* getInputSystem();

protected:
	virtual void onCreate() {}
	virtual void onResize() {}
	virtual void onUpdate(float deltaTime) {}
	virtual void onQuit() {}

private:
	void calculateFrameStats();
	void onDisplaySize(int width, int height);
	void onInternalUpdate();
	void setPause(bool isPaused);

	std::unique_ptr<InputSystem> p_inputSystem;
	std::unique_ptr<Display> p_display;
	std::unique_ptr<GameTimer> p_timer;
	std::unique_ptr<GraphicsEngine> p_graphicsEngine;

	bool p_isRunning = true;
	bool p_isPaused = false;

	friend class GraphicsEngine;
	friend class Display;
};

