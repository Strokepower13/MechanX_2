#pragma once
#include <DX3D/AllHeaders.h>
#include <DirectXMath.h>
#include <DirectXColors.h>

class BoxApp :public Game
{
public:
	BoxApp(HINSTANCE hInstance);
	~BoxApp();

protected:
	virtual void onCreate();
	virtual void onUpdate(float deltaTime);

private:
	ConstantBufferPtr p_cb = nullptr;
	VertexBufferPtr p_vb = nullptr;
	IndexBufferPtr p_ib = nullptr;

	VertexShaderPtr p_vs = nullptr;
	PixelShaderPtr p_ps = nullptr;

	InputLayoutPtr p_inputLayout = nullptr;

	PipelineStatePtr p_pso = nullptr;

	DirectX::XMFLOAT4X4 p_world;
	DirectX::XMFLOAT4X4 p_view;
	DirectX::XMFLOAT4X4 p_proj;

	float p_theta = 1.5f * DirectX::XM_PI;
	float p_phi = DirectX::XM_PIDIV4;
	float p_radius = 5.0f;
};

