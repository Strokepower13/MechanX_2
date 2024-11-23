#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <DX3D/Prerequisites.h>

class PipelineState
{
public:
	PipelineState(const InputLayoutPtr& inputLayout, const VertexShaderPtr& vs, const PixelShaderPtr& ps, RenderSystem* system);
	~PipelineState();

private:
	Microsoft::WRL::ComPtr<ID3D12PipelineState> p_PSO;
	RenderSystem* p_system = nullptr;

	friend class CommandManager;
};

