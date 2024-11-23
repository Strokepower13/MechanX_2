#include <DX3D/Graphics/CommandManager.h>
#include <DX3D/Graphics/SwapChain.h>
#include <DX3D/Graphics/RenderSystem.h>
#include <DX3D/Graphics/DescriptorHeap.h>
#include <DX3D/Graphics/RootSignature.h>
#include <DX3D/Graphics/PipelineState.h>
#include <DX3D/Graphics/VertexBuffer.h>
#include <DX3D/Graphics/IndexBuffer.h>
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

	hr = p_commandList->Close();
	if (FAILED(hr))
		DX3DError("CommandManager not created successfully.");
}

CommandManager::~CommandManager()
{
}

void CommandManager::reset()
{
	HRESULT hr = p_directCmdListAlloc->Reset();
	if (FAILED(hr))
		DX3DError("CommandManager::reset error.");

	hr = p_commandList->Reset(p_directCmdListAlloc.Get(), p_PSO.Get());
	if (FAILED(hr))
		DX3DError("CommandManager::reset error.");
}

void CommandManager::resetCmdList()
{
	HRESULT hr = p_commandList->Reset(p_directCmdListAlloc.Get(), nullptr);
	if (FAILED(hr))
		DX3DError("CommandManager::resetCmdList error.");
}

void CommandManager::closeCmdList()
{
	HRESULT hr = p_commandList->Close();
	if (FAILED(hr))
		DX3DError("CommandManager::closeCmdList error.");

	ID3D12CommandList* cmdsLists[] = { p_commandList.Get() };
	p_commandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	this->flushCommandQueue();
}

void CommandManager::setViewportSize(const SwapChainPtr& swapChain)
{
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
}

void CommandManager::clearRenderTargetColor(const SwapChainPtr& swapChain, DirectX::XMVECTORF32 color)
{
	auto rtv = p_system->p_descriptorHeap->currentBackBufferView(swapChain);
	auto dsv = p_system->p_descriptorHeap->depthStencilView();

	p_commandList->ClearRenderTargetView(rtv, color, 0, nullptr);
	p_commandList->ClearDepthStencilView(dsv, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
	p_commandList->OMSetRenderTargets(1, &rtv, true, &dsv);
}

void CommandManager::begin(const SwapChainPtr& swapChain)
{
	this->reset();
	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(swapChain->currentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	p_commandList->ResourceBarrier(1, &barrier);
}

void CommandManager::finish(const SwapChainPtr& swapChain)
{
	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(swapChain->currentBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	p_commandList->ResourceBarrier(1, &barrier);

	HRESULT hr = p_commandList->Close();
	if (FAILED(hr))
		DX3DError("CommandManager::finish error.");

	ID3D12CommandList* cmdsLists[] = { p_commandList.Get() };
	p_commandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);
}

void CommandManager::setPSO(const PipelineStatePtr& PSO)
{
	this->p_PSO = PSO->p_PSO;
}

void CommandManager::setDescriptorHeaps()
{
	ID3D12DescriptorHeap* descriptorHeaps[] = { p_system->p_descriptorHeap->p_cbvHeap.Get() };
	p_commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
}

void CommandManager::setRootSignature()
{
	p_commandList->SetGraphicsRootSignature(p_system->p_rootSignature->p_rootSignature.Get());
}

void CommandManager::setVertexBuffer(const VertexBufferPtr& vertexBuffer)
{
	auto vbv = vertexBuffer->getVBV();
	p_commandList->IASetVertexBuffers(0, 1, &vbv);
}

void CommandManager::setIndexBuffer(const IndexBufferPtr& indexBuffer)
{
	auto ibv = indexBuffer->getIBV();
	p_commandList->IASetIndexBuffer(&ibv);
}

void CommandManager::setDescriptorTable()
{
	p_commandList->SetGraphicsRootDescriptorTable(0, p_system->p_descriptorHeap->p_cbvHeap->GetGPUDescriptorHandleForHeapStart());
}

void CommandManager::drawIndexedTriangleList(UINT indexCount, UINT startIndexLocation, UINT startVertexIndex)
{
	p_commandList->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	p_commandList->DrawIndexedInstanced(indexCount, 1, startIndexLocation, startVertexIndex, 0);
}

void CommandManager::flushCommandQueue()
{
	p_currentFence++;
	HRESULT hr = p_commandQueue->Signal(p_fence.Get(), p_currentFence);
	if (FAILED(hr))
		DX3DError("CommandManager::flushCommandQueue error.");

	if (p_fence->GetCompletedValue() < p_currentFence)
	{
		HANDLE eventHandle = CreateEventEx(nullptr, NULL, false, EVENT_ALL_ACCESS);

		hr = p_fence->SetEventOnCompletion(p_currentFence, eventHandle);
		if (FAILED(hr))
			DX3DError("CommandManager::flushCommandQueue error.");

		if(eventHandle)
		{
			WaitForSingleObject(eventHandle, INFINITE);
			CloseHandle(eventHandle);
		}
	}
}
