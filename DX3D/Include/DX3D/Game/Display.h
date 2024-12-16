#pragma once
#include <DX3D/Prerequisites.h>
#include <DX3D/Window/Window.h>

class Display : public Window
{
public:
	Display(HINSTANCE hInstance, Game* game);
	~Display();
	SwapChainPtr getSwapChain();

protected:
	virtual void onFocus();
	virtual void onKillFocus();
	virtual void onSize();

private:
	SwapChainPtr p_swapChain;
	MSAAResourcesPtr p_msaaRes;
	Game* p_game = nullptr;

	friend class GraphicsEngine;
};