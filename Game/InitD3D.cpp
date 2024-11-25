#include "InitD3D.h"
#include <DirectXColors.h>
#include <DX3D/Graphics/ConstantBuffer.h>

struct ObjectConstants
{
	DirectX::XMFLOAT4X4 WorldViewProj = DirectX::XMFLOAT4X4(1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);
};

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

	ObjectConstants ass;
	//ConstantBuffer sas(sizeof(ObjectConstants), 1, getGraphicsEngine()->getRenderSystem());
	ConstantBufferPtr sas = nullptr;
	sas = getGraphicsEngine()->getRenderSystem()->createConstantBuffer(sizeof(ObjectConstants), 1);
	sas->update(0, &ass);

	VertexShaderPtr pop = getGraphicsEngine()->getRenderSystem()->createVertexShader(L"Assets/Shaders/Color.cso");
	
	commMgr->begin();
	commMgr->setViewportSize();
	//commMgr->clearRenderTargetColor(scptr, 0.5f, 0.0f, 0.0f, 1.0f);
	commMgr->clearRenderTargetColor(DirectX::Colors::LightBlue);
	commMgr->finish();

	scptr->present(true);
}
