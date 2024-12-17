#include <DX3D/Graphics/PipelineState.h>
#include <DX3D/Graphics/RenderSystem.h>
#include <DX3D/Graphics/InputLayout.h>
#include <DX3D/Graphics/RootSignature.h>
#include <DX3D/Shaders/VertexShader.h>
#include <DX3D/Shaders/PixelShader.h>

#include <d3dx12.h>

PipelineState::PipelineState(const InputLayoutPtr& inputLayout, const VertexShaderPtr& vs, const PixelShaderPtr& ps, RenderSystem* system) : p_system(system)
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
	ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	psoDesc.InputLayout = { inputLayout->p_inputLayout.data(), (UINT)inputLayout->p_inputLayout.size() };
	psoDesc.pRootSignature = p_system->p_rootSignature->p_rootSignature.Get();
	psoDesc.VS = { vs->p_blob.data(), vs->p_blob.size() };
	psoDesc.PS = { ps->p_blob.data(), ps->p_blob.size() };

	CD3DX12_RASTERIZER_DESC rastDesc(D3D12_FILL_MODE_SOLID,
		D3D12_CULL_MODE_BACK, FALSE,
		D3D12_DEFAULT_DEPTH_BIAS, D3D12_DEFAULT_DEPTH_BIAS_CLAMP,
		D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS, TRUE, TRUE, FALSE,
		0, D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF);
	//CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);

	psoDesc.RasterizerState = p_system->p_4xMsaaState ? rastDesc : CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = p_system->p_backBufferFormat;
	psoDesc.SampleDesc.Count = p_system->p_4xMsaaState ? 4 : 1;
	psoDesc.SampleDesc.Quality = p_system->p_4xMsaaState ? (p_system->p_4xMsaaQuality - 1) : 0;
	psoDesc.DSVFormat = p_system->p_depthStencilFormat;
	
	HRESULT hr = p_system->p_d3dDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&p_PSO));
	if (FAILED(hr))
		DX3DError("PipelineState not created successfully.");
}

PipelineState::~PipelineState()
{
}
