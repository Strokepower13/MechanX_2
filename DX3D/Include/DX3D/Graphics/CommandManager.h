#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <DX3D/Prerequisites.h>
#include <DirectXColors.h>

class CommandManager
{
public:
	CommandManager(RenderSystem* system);
	~CommandManager();

public:
	void resetCmdList();
	void closeCmdList();

	void setViewportSize(const SwapChainPtr& swapChain);
	void clearRenderTargetColor(const SwapChainPtr& swapChain, float red, float green, float blue, float alpha);
	void clearRenderTargetColor(const SwapChainPtr& swapChain, DirectX::XMVECTORF32 color);

	void begin(const SwapChainPtr& swapChain);
	void finish(const SwapChainPtr& swapChain);

	void setPSO(const PipelineStatePtr& PSO);

	void setDescriptorHeaps();
	void setRootSignature();

	void setVertexBuffer(const VertexBufferPtr& vertexBuffer);
	void setIndexBuffer(const IndexBufferPtr& indexBuffer);

	void setDescriptorTable();

	void drawIndexedTriangleList(UINT indexCount, UINT startIndexLocation, UINT startVertexIndex);

protected:
	void reset();
	void flushCommandQueue();

private:
	RenderSystem* p_system = nullptr;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> p_commandQueue;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> p_directCmdListAlloc;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> p_commandList;

	Microsoft::WRL::ComPtr<ID3D12Fence> p_fence;
	UINT64 p_currentFence = 0;

	Microsoft::WRL::ComPtr<ID3D12PipelineState> p_PSO = nullptr;

	friend class SwapChain;
	friend class RenderSystem;
	friend class VertexBuffer;
	friend class IndexBuffer;
};

