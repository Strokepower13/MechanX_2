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

	D3D12_PRIMITIVE_TOPOLOGY primitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	UINT indexCount = 0;
	UINT startIndexLocation = 0;
	int baseVertexLocation = 0;
};

enum class RenderLayer : int
{
	opaque = 0,
	count
};

class WavesDemo :public Game
{
public:
	WavesDemo(HINSTANCE hInstance);
	~WavesDemo();

protected:
	virtual void onCreate();
	virtual void onResize();
	virtual void onUpdate(float deltaTime);

private:
	float getHillsHeight(float x, float z)const;
	
	std::vector<std::unique_ptr<FrameResource>> p_frameResources;
	FrameResource* p_currFrameResource = nullptr;
	int p_currFrameResourceIndex = 0;

	InputLayoutPtr p_inputLayout = nullptr;

	std::unordered_map<std::string, std::unique_ptr<Mesh>> p_meshes;
	std::unordered_map<std::string, VertexShaderPtr> p_VSs;
	std::unordered_map<std::string, PixelShaderPtr> p_PSs;
	std::unordered_map<std::string, PipelineStatePtr> p_PSOs;

	RenderItem* p_wavesRitem = nullptr;
	std::vector<std::unique_ptr<RenderItem>> p_allRitems;
	std::vector<RenderItem*> p_ritemLayer[(int)RenderLayer::count];

	std::unique_ptr<Waves> p_waves;

	PassConstants p_mainPassCB;

	bool p_isWireframe = false;

	DirectX::XMFLOAT3 p_eyePos = { 0.0f,0.0f,0.0f };
	DirectX::XMFLOAT4X4 p_view;
	DirectX::XMFLOAT4X4 p_proj;

	float p_theta = 1.5f * DirectX::XM_PI;
	float p_phi = DirectX::XM_PIDIV2 - 0.1f;
	float p_radius = 50.0f;

	float p_sunTheta = 1.25f * DirectX::XM_PI;
	float p_sunPhi = DirectX::XM_PIDIV4;
};

