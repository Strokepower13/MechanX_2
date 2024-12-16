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
	void resize();

	void setFence(UINT64& fence);
	void signal(UINT64& fence);

	void setViewportSize();
	void clearRenderTargetColor(float red, float green, float blue, float alpha);
	void clearRenderTargetColor(DirectX::XMVECTORF32 color);

	void begin();
	void begin(ID3D12CommandAllocator* alloc);
	void finish();

	void setPSO(const PipelineStatePtr& PSO);

	void setDescriptorHeaps();
	void setRootSignature();

	void setVertexBuffer(const VertexBufferPtr& vertexBuffer);
	void setIndexBuffer(const IndexBufferPtr& indexBuffer);

	void setDescriptorTable();
	void setDescriptorTable(UINT rootParameter, int cbvOffset);
	void setCBV(UINT rootParameter, const ConstantBufferPtr& constantBuffer);
	void setCBV(UINT rootParameter, UINT cbIndex, const ConstantBufferPtr& constantBuffer);

	void drawIndexedTriangleList(UINT indexCount, UINT startIndexLocation, UINT startVertexIndex);
	void flushCommandQueue();

protected:
	void reset();

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

