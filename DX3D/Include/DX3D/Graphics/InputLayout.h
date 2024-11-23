#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <DX3D/Prerequisites.h>
#include <vector>

class InputLayout
{
public:
	InputLayout(InputLayoutType type);

private:
	std::vector<D3D12_INPUT_ELEMENT_DESC> p_inputLayout;

	friend class PipelineState;
};

