#pragma once
#include <DX3D/AllHeaders.h>
#include <DirectXMath.h>
#include <DirectXColors.h>
#include <d3dx12.h>

const int gNumFrameResources = 3;

struct RenderItem
{
	RenderItem() = default;

	DirectX::XMFLOAT4X4 world{};

	int numFramesDirty = gNumFrameResources;

	UINT objCBIndex = -1;

	Mesh* mesh = nullptr;

	D3D12_PRIMITIVE_TOPOLOGY primitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	UINT indexCount = 0;
	UINT startIndexLocation = 0;
	int baseVertexLocation = 0;
};

class Shapes :public Game
{
public:
	Shapes(HINSTANCE hInstance);
	~Shapes();

protected:
	virtual void onCreate();
	virtual void onResize();
	virtual void onUpdate(float deltaTime);

private:
	ConstantBufferPtr p_cb = nullptr;
	
	std::unique_ptr<Mesh> p_mesh;
	std::vector<std::unique_ptr<RenderItem>> p_allRitems;
	std::vector<RenderItem*> p_opaqueRitems;
	std::vector<std::unique_ptr<FrameResource>> p_frameResources;
	FrameResource* p_currFrameResource = nullptr;
	int p_currFrameResourceIndex = 0;
	PassConstants p_mainPassCB;
	UINT p_passCbvOffset = 0;

	VertexShaderPtr p_vs = nullptr;
	PixelShaderPtr p_ps = nullptr;

	InputLayoutPtr p_inputLayout = nullptr;

	PipelineStatePtr p_pso = nullptr;

	DirectX::XMFLOAT3 p_eyePos = { 0.0f,0.0f,0.0f };
	DirectX::XMFLOAT4X4 p_world;
	DirectX::XMFLOAT4X4 p_view;
	DirectX::XMFLOAT4X4 p_proj;

	float p_theta = 1.5f * DirectX::XM_PI;
	float p_phi = DirectX::XM_PI * 0.2f;
	float p_radius = 15.0f;
};

