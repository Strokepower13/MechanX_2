#include <DX3D/Graphics/CommandManager.h>
#include <DX3D/Graphics/SwapChain.h>
#include <DX3D/Graphics/RenderSystem.h>
#include <DX3D/Graphics/DescriptorHeap.h>
#include <d3dx12.h>

CommandManager::CommandManager(RenderSystem* system) : p_system(system)
{
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

	auto& device = p_system->p_d3dDevice;

	HRESULT hr = device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&p_commandQueue));
	if (FAILED(hr))
		DX3DError("CommandManager not created successfully.");

	hr = device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&p_fence));
	if (FAILED(hr))
		DX3DError("CommandManager not created successfully.");

	hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(p_directCmdListAlloc.GetAddressOf()));
	if (FAILED(hr))
		DX3DError("CommandManager not created successfully.");

	hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, p_directCmdListAlloc.Get(), nullptr, IID_PPV_ARGS(p_commandList.GetAddressOf()));
	if (FAILED(hr))
		DX3DError("CommandManager not created successfully.");

	p_commandList->Close();
}

CommandManager::~CommandManager()
{
}

void CommandManager::reset(const SwapChainPtr& swapChain)
{
	p_directCmdListAlloc->Reset();
	p_commandList->Reset(p_directCmdListAlloc.Get(), nullptr);

	//D3D12_RESOURCE_BARRIER resBar;
	//ZeroMemory(&resBar, sizeof(resBar));
	//resBar.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	//resBar.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	//resBar.Transition.pResource = swapChain->currentBackBuffer();
	//resBar.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	//resBar.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	//resBar.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(swapChain->currentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	p_commandList->ResourceBarrier(1, &barrier);
}

void CommandManager::setViewportSize(const SwapChainPtr& swapChain)
{
	this->reset(swapChain);

	p_commandList->RSSetViewports(1, &swapChain->p_screenViewport);
	p_commandList->RSSetScissorRects(1, &swapChain->p_scissorRect);
}

void CommandManager::clearRenderTargetColor(const SwapChainPtr& swapChain, float red, float green, float blue, float alpha)
{
	FLOAT clearColor[] = { red,green,blue,alpha };

	auto rtv = p_system->p_descriptorHeap->currentBackBufferView(swapChain);
	auto dsv = p_system->p_descriptorHeap->depthStencilView();

	p_commandList->ClearRenderTargetView(rtv, clearColor, 0, nullptr);
	p_commandList->ClearDepthStencilView(dsv, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
	p_commandList->OMSetRenderTargets(1, &rtv, true, &dsv);

	//D3D12_RESOURCE_BARRIER resBar;
	//ZeroMemory(&resBar, sizeof(resBar));
	//resBar.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	//resBar.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	//resBar.Transition.pResource = swapChain->currentBackBuffer();
	//resBar.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	//resBar.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	//resBar.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(swapChain->currentBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	p_commandList->ResourceBarrier(1, &barrier);

	p_commandList->Close();

	ID3D12CommandList* cmdsLists[] = { p_commandList.Get() };
	p_commandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	swapChain->present(true);
}

void CommandManager::flushCommandQueue()
{
	p_currentFence++;
	HRESULT hr = p_commandQueue->Signal(p_fence.Get(), p_currentFence);
	if (FAILED(hr))
		DX3DError("flushCommandQueue error.");

	if (p_fence->GetCompletedValue() < p_currentFence)
	{
		HANDLE eventHandle = CreateEventEx(nullptr, NULL, false, EVENT_ALL_ACCESS);

		hr = p_fence->SetEventOnCompletion(p_currentFence, eventHandle);
		if (FAILED(hr))
			DX3DError("flushCommandQueue error.");

		if(eventHandle)
		{
			WaitForSingleObject(eventHandle, INFINITE);
			CloseHandle(eventHandle);
		}
	}
}
