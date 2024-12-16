#include <DX3D/Game/Display.h>
#include <DX3D/Game/Game.h>
#include <DX3D/Graphics/GraphicsEngine.h>
#include <DX3D/Graphics/RenderSystem.h>
#include <DX3D/Graphics/SwapChain.h>
#include <DX3D/Graphics/MSAAResources.h>

Display::Display(HINSTANCE hInstance, Game* game) :Window(hInstance), p_game(game)
{
	auto rs = p_game->getGraphicsEngine()->getRenderSystem();
	p_swapChain = rs->createSwapChain(p_hWnd, p_clientWidth, p_clientHeight);
	p_msaaRes = rs->createMSAAResources(p_clientWidth, p_clientHeight);
}

Display::~Display()
{
}

void Display::onFocus()
{
	p_game->setPause(false);
}

void Display::onKillFocus()
{
	p_game->setPause(true);
}

void Display::onSize()
{
	p_swapChain->resize((p_clientWidth > 1) ? p_clientWidth : 1, (p_clientHeight > 1) ? p_clientHeight : 1);
	//if (p_game->getGraphicsEngine()->getRenderSystem()->getMSAAState())
	p_msaaRes->resize((p_clientWidth > 1) ? p_clientWidth : 1, (p_clientHeight > 1) ? p_clientHeight : 1);
	p_game->onDisplaySize(p_clientWidth, p_clientHeight);
}

SwapChainPtr Display::getSwapChain()
{
	return this->p_swapChain;
}
