#include <DX3D/Graphics/CommandManager.h>
#include <DX3D/Graphics/SwapChain.h>
#include <DX3D/Graphics/RenderSystem.h>
#include <DX3D/Graphics/DescriptorHeap.h>
#include <DX3D/Graphics/RootSignature.h>
#include <DX3D/Graphics/PipelineState.h>
#include <DX3D/Buffers/ConstantBuffer.h>
#include <DX3D/Buffers/IndexBuffer.h>
#include <DX3D/Buffers/VertexBuffer.h>
#include <d3dx12.h>

#include <DX3D/Graphics/MSAAResources.h>

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
}

void CommandManager::resize()
{
	reset();
	closeCmdList();
}

void CommandManager::setFence(UINT64& fence)
{
	if (fence != 0 && p_fence->GetCompletedValue() < fence)
	{
		HANDLE eventHandle = CreateEventEx(nullptr, NULL, false, EVENT_ALL_ACCESS);
		HRESULT hr = p_fence->SetEventOnCompletion(fence, eventHandle);
		if (FAILED(hr))
			DX3DError("CommandManager::fk error.");

		if(eventHandle)
		{
			WaitForSingleObject(eventHandle, INFINITE);
			CloseHandle(eventHandle);
		}
	}
}

void CommandManager::signal(UINT64& fence)
{
	fence = ++p_currentFence;
	p_commandQueue->Signal(p_fence.Get(), p_currentFence);
}

void CommandManager::setViewportSize()
{
	auto& sc = p_system->p_swapChain;
	p_commandList->RSSetViewports(1, &sc->p_screenViewport);
	p_commandList->RSSetScissorRects(1, &sc->p_scissorRect);
}

void CommandManager::clearRenderTargetColor(float red, float green, float blue, float alpha)
{
	FLOAT clearColor[] = { red,green,blue,alpha };

	D3D12_CPU_DESCRIPTOR_HANDLE  rtv{};
	D3D12_CPU_DESCRIPTOR_HANDLE  dsv{};

	if (p_system->p_4xMsaaState)
	{
		auto& msaaRes = p_system->p_msaaRes;
		rtv = msaaRes->p_RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		dsv = msaaRes->p_DSVDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	}
	else
	{
		auto& descriptorHeap = p_system->p_descriptorHeap;
		rtv = descriptorHeap->currentBackBufferView(p_system->p_swapChain);
		dsv = descriptorHeap->depthStencilView();
	}

	p_commandList->ClearRenderTargetView(rtv, clearColor, 0, nullptr);
	p_commandList->ClearDepthStencilView(dsv, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
	p_commandList->OMSetRenderTargets(1, &rtv, true, &dsv);
}

void CommandManager::clearRenderTargetColor(DirectX::XMVECTORF32 color)
{
	D3D12_CPU_DESCRIPTOR_HANDLE  rtv{};
	D3D12_CPU_DESCRIPTOR_HANDLE  dsv{};

	if (p_system->p_4xMsaaState)
	{
		auto& msaaRes = p_system->p_msaaRes;
		rtv = msaaRes->p_RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		dsv = msaaRes->p_DSVDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	}
	else
	{
		auto& descriptorHeap = p_system->p_descriptorHeap;
		rtv = descriptorHeap->currentBackBufferView(p_system->p_swapChain);
		dsv = descriptorHeap->depthStencilView();
	}

	p_commandList->ClearRenderTargetView(rtv, color, 0, nullptr);
	p_commandList->ClearDepthStencilView(dsv, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
	p_commandList->OMSetRenderTargets(1, &rtv, true, &dsv);
}

void CommandManager::begin()
{
	this->reset();

	if (p_system->p_4xMsaaState)
	{
		auto& msaaRes = p_system->p_msaaRes;
		auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(msaaRes->p_renderTarget.Get(), D3D12_RESOURCE_STATE_RESOLVE_SOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);
		p_commandList->ResourceBarrier(1, &barrier);
	}

	else
	{
		auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(p_system->p_swapChain->currentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		p_commandList->ResourceBarrier(1, &barrier);
	}
}

void CommandManager::begin(ID3D12CommandAllocator* alloc)
{
	HRESULT hr = alloc->Reset();
	if (FAILED(hr))
		DX3DError("CommandManager::begin error.");

	hr = p_commandList->Reset(alloc, p_PSO.Get());
	if (FAILED(hr))
		DX3DError("CommandManager::begin error.");

	if (p_system->p_4xMsaaState)
	{
		auto& msaaRes = p_system->p_msaaRes;
		auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(msaaRes->p_renderTarget.Get(), D3D12_RESOURCE_STATE_RESOLVE_SOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);
		p_commandList->ResourceBarrier(1, &barrier);
	}

	else
	{
		auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(p_system->p_swapChain->currentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		p_commandList->ResourceBarrier(1, &barrier);
	}
}

void CommandManager::finish()
{
	auto backBuffer = p_system->p_swapChain->currentBackBuffer();
	if (p_system->p_4xMsaaState)
	{
		auto& msaaRes = p_system->p_msaaRes;
		D3D12_RESOURCE_BARRIER  barriers[2] = {
			CD3DX12_RESOURCE_BARRIER::Transition(msaaRes->p_renderTarget.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_RESOLVE_SOURCE),
			CD3DX12_RESOURCE_BARRIER::Transition(backBuffer, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RESOLVE_DEST)
		};
		p_commandList->ResourceBarrier(2, barriers);
		p_commandList->ResolveSubresource(backBuffer, 0, msaaRes->p_renderTarget.Get(), 0, p_system->p_backBufferFormat);

		auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(backBuffer, D3D12_RESOURCE_STATE_RESOLVE_DEST, D3D12_RESOURCE_STATE_PRESENT);
		p_commandList->ResourceBarrier(1, &barrier);
	}
	else
	{
		auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(backBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		p_commandList->ResourceBarrier(1, &barrier);
	}

	closeCmdList();
}

void CommandManager::setPSO(const PipelineStatePtr& PSO)
{
	flushCommandQueue();
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

void CommandManager::setDescriptorTable(UINT rootParameter, int cbvOffset)
{
	auto passCbvHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(p_system->p_descriptorHeap->p_cbvHeap->GetGPUDescriptorHandleForHeapStart());
	passCbvHandle.Offset(cbvOffset, p_system->p_descriptorHeap->p_cbvSrvUavDescriptorSize);
	p_commandList->SetGraphicsRootDescriptorTable(rootParameter, passCbvHandle);
}

void CommandManager::setCBV(UINT rootParameter, const ConstantBufferPtr& constantBuffer)
{
	p_commandList->SetGraphicsRootConstantBufferView(rootParameter, constantBuffer->p_buffer->GetGPUVirtualAddress());
}

void CommandManager::setCBV(UINT rootParameter, UINT cbIndex, const ConstantBufferPtr& constantBuffer)
{
	D3D12_GPU_VIRTUAL_ADDRESS objCBAddress = constantBuffer->p_buffer->GetGPUVirtualAddress();
	objCBAddress += static_cast<unsigned long long>(cbIndex) * constantBuffer->p_elementByteSize;
	p_commandList->SetGraphicsRootConstantBufferView(rootParameter, objCBAddress);
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
