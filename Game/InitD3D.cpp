#include "InitD3D.h"

InitD3D::InitD3D(HINSTANCE hInstance) : Game(hInstance)
{

}

InitD3D::~InitD3D()
{
}

void InitD3D::onCreate()
{
}

void InitD3D::onUpdate(float deltaTime)
{
	auto commMgr = getGraphicsEngine()->getRenderSystem()->getCommandMgr();
	
	auto scptr = getDisplay()->getSwapChain();

	commMgr->setViewportSize(scptr);
	commMgr->clearRenderTargetColor(scptr, 0.5f, 0.0f, 0.0f, 1.0f);
}
