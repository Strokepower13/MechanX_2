#include <DX3D/Game/Game.h>
#include <DX3D/Window/Window.h>
#include <DX3D/Game/GameTimer.h>
#include <DX3D/Graphics/GraphicsEngine.h>
#include <DX3D/Game/Display.h>
#include <DX3D/Input/InputSystem.h>
#include <sstream>

Game::Game(HINSTANCE hInstance)
{
	p_graphicsEngine = std::make_unique<GraphicsEngine>(this);
	p_display = std::make_unique<Display>(hInstance, this);
	p_timer = std::make_unique<GameTimer>();
	p_inputSystem = std::make_unique<InputSystem>(p_display->getHwnd());
}

Game::~Game()
{
}

void Game::run()
{
	onCreate();
	onResize();

	MSG msg = {};

	p_timer->reset();
	p_timer->start();
	while (p_isRunning)
	{
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				p_isRunning = false;
				continue;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		p_timer->tick();
		calculateFrameStats();
		onInternalUpdate();
	}
	p_timer->stop();
	onQuit();
}

void Game::quit()
{
	p_isRunning = false;
}

Display* Game::getDisplay()
{
	return p_display.get();
}

GameTimer* Game::getTimer()
{
	return p_timer.get();
}

GraphicsEngine* Game::getGraphicsEngine()
{
	return p_graphicsEngine.get();
}

InputSystem* Game::getInputSystem()
{
	return p_inputSystem.get();
}

void Game::calculateFrameStats()
{
	static int frameCnt = 0;
	static float timeElapsed = 0.0f;
	frameCnt++;

	if ((p_timer->totalTime() - timeElapsed) >= 1.0f)
	{
		float fps = (float)frameCnt;
		float mspf = 1000.0f / fps;

		std::wostringstream outs;
		outs.precision(6);
		outs << L"MechanX Application" << L" "
			<< L"FPS: " << fps << L" "
			<< L"Frame Time: " << mspf << L"ms";
		auto hwnd = p_display->getHwnd();
		SetWindowText(hwnd, outs.str().c_str());

		frameCnt = 0;
		timeElapsed += 1.0f;
	}
}

void Game::onDisplaySize(int width, int height)
{
	onResize();
	onInternalUpdate();
}

void Game::onInternalUpdate()
{
	p_inputSystem->update();
	onUpdate(p_timer.get()->deltaTime());
	//p_graphicsEngine->update();
}