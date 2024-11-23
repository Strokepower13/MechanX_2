#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <DX3D/Prerequisites.h>

class RootSignature
{
public:
	RootSignature(RenderSystem* system);
	~RootSignature();

private:
	Microsoft::WRL::ComPtr<ID3D12RootSignature> p_rootSignature;
	RenderSystem* p_system;

	friend class PipelineState;
	friend class CommandManager;
};

