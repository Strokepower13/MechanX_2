#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <DX3D/Prerequisites.h>

class CommandManager
{
public:
	CommandManager(RenderSystem* system);
	~CommandManager();

public:
	void reset(const SwapChainPtr& swapChain);
	void setViewportSize(const SwapChainPtr& swapChain);
	void clearRenderTargetColor(const SwapChainPtr& swapChain, float red, float green, float blue, float alpha);

protected:
	void flushCommandQueue();

private:
	RenderSystem* p_system = nullptr;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> p_commandQueue;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> p_directCmdListAlloc;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> p_commandList;

	Microsoft::WRL::ComPtr<ID3D12Fence> p_fence;
	UINT64 p_currentFence = 0;

	friend class SwapChain;
};

