#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <DX3D/Prerequisites.h>

class DescriptorHeap
{
public:
	DescriptorHeap(RenderSystem* system);
	~DescriptorHeap();

private:
	D3D12_CPU_DESCRIPTOR_HANDLE depthStencilView() const;
	D3D12_CPU_DESCRIPTOR_HANDLE currentBackBufferView(const SwapChainPtr& swapChain) const;
	
	RenderSystem* p_system = nullptr;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> p_rtvHeap;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> p_dsvHeap;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> p_cbvHeap;

	UINT p_rtvDescriptorSize = 0;
	UINT p_dsvDescriptorSize = 0;
	UINT p_cbvSrvUavDescriptorSize = 0;

	friend class SwapChain;
	friend class CommandManager;
	friend class ConstantBuffer;
};

