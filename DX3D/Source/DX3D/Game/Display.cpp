#include <DX3D/Game/Display.h>
#include <DX3D/Game/Game.h>
#include <DX3D/Graphics/GraphicsEngine.h>
#include <DX3D/Graphics/RenderSystem.h>
#include<DX3D/Graphics/SwapChain.h>

Display::Display(HINSTANCE hInstance, Game* game) :Window(hInstance), p_game(game)
{
	p_swapChain = game->getGraphicsEngine()->getRenderSystem()->createSwapChain(p_hWnd, p_clientWidth, p_clientHeight);
}

Display::~Display()
{
}

void Display::onSize()
{
	p_swapChain->resize((p_clientWidth > 1) ? p_clientWidth : 1, (p_clientHeight > 1) ? p_clientHeight : 1);
	p_game->onDisplaySize(p_clientWidth, p_clientHeight);
}

SwapChainPtr Display::getSwapChain()
{
	return this->p_swapChain;
}
