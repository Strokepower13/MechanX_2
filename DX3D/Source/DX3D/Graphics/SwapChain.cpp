#include <DX3D/Graphics/SwapChain.h>
#include <DX3D/Graphics/RenderSystem.h>
#include <DX3D/Graphics/CommandManager.h>
#include <DX3D/Graphics/DescriptorHeap.h>
#include <exception>
#include <sstream>
#include <d3dx12.h>

SwapChain::SwapChain(HWND hwnd, UINT width, UINT height, RenderSystem* system, bool enable4xMsaa) :p_system(system),
p_enable4xMsaa(enable4xMsaa)
{
	//p_swapChain.Reset();

	p_swapChainBufferCount = p_system->p_swapChainBufferCount;
	p_swapChainBuffer.resize(p_swapChainBufferCount);

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferDesc.Width = width;
	sd.BufferDesc.Height = height;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = p_system->p_backBufferFormat;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.SampleDesc.Count = p_system->p_4xMsaaState ? 4 : 1;
	sd.SampleDesc.Quality = p_system->p_4xMsaaState ? (p_system->p_4xMsaaQuality - 1) : 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = p_swapChainBufferCount;
	sd.OutputWindow = hwnd;
	sd.Windowed = true;
	sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	HRESULT hr = p_system->p_dxgiFactory->CreateSwapChain(p_system->p_commandMgr->p_commandQueue.Get(), &sd, p_swapChain.GetAddressOf());
	if (FAILED(hr))
		DX3DError("SwapChain not created successfully.");

	if(p_swapChain==nullptr)
		DX3DError("SwapChain not created successfully.");
	
	//reloadBuffers(width, height);
	resize(width, height);
}

void SwapChain::setFullscreen(bool fullscreen, unsigned int width, unsigned int height)
{
	resize(width, height);
	//p_swapChain->SetFullscreenState(fullscreen, nullptr);
}

void SwapChain::resize(unsigned int width, unsigned int height)
{
	auto& commandMgr = p_system->p_commandMgr;

	HRESULT hr = commandMgr->p_commandList->Reset(commandMgr->p_directCmdListAlloc.Get(), nullptr);
	if (FAILED(hr))
		DX3DError("SwapChain not created successfully.");

	for (int i = 0; i < p_swapChainBufferCount; ++i)
		p_swapChainBuffer[i].Reset();
	p_depthStencilBuffer.Reset();

	hr = p_swapChain->ResizeBuffers(p_swapChainBufferCount, width, height, p_system->p_backBufferFormat, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
	if (FAILED(hr))
		DX3DError("SwapChain not created successfully.");
	
	p_currBackBuffer = 0;

	//auto rtvHeapHandle = p_system->p_descriptorHeap->renderTargetView();
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(p_system->p_descriptorHeap->p_rtvHeap->GetCPUDescriptorHandleForHeapStart());
	for (UINT i = 0; i < p_swapChainBufferCount; i++)
	{
		hr = p_swapChain->GetBuffer(i, IID_PPV_ARGS(&p_swapChainBuffer[i]));
		if (FAILED(hr))
			DX3DError("SwapChain not created successfully.");

		p_system->p_d3dDevice->CreateRenderTargetView(p_swapChainBuffer[i].Get(), nullptr, rtvHeapHandle);
		//rtvHeapHandle.ptr += (1 * p_system->p_descriptorHeap->p_rtvDescriptorSize);
		rtvHeapHandle.Offset(1, p_system->p_descriptorHeap->p_rtvDescriptorSize);
	}

	reloadBuffers(width, height);
}

void SwapChain::present(bool vsync)
{
	p_swapChain->Present(vsync, NULL);

	p_currBackBuffer = (p_currBackBuffer + 1) % p_swapChainBufferCount;

	p_system->p_commandMgr->flushCommandQueue();
}

void SwapChain::reloadBuffers(unsigned int width, unsigned int height)
{
	p_system->p_commandMgr->flushCommandQueue();

	auto& device = p_system->p_d3dDevice;

	D3D12_RESOURCE_DESC depthStencilDesc{};
	depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthStencilDesc.Alignment = 0;
	depthStencilDesc.Width = width;
	depthStencilDesc.Height = height;
	depthStencilDesc.DepthOrArraySize = 1;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = p_system->p_4xMsaaState ? 4 : 1;
	depthStencilDesc.SampleDesc.Quality = p_system->p_4xMsaaState ? (p_system->p_4xMsaaQuality - 1) : 0;
	depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE optClear{};
	optClear.Format = p_system->p_depthStencilFormat;
	optClear.DepthStencil.Depth = 1.0f;
	optClear.DepthStencil.Stencil = 0;

	//D3D12_HEAP_PROPERTIES heapProps;
	//heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
	//heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	//heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	//heapProps.CreationNodeMask = 1;
	//heapProps.VisibleNodeMask = 1;

	auto heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	HRESULT hr = device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &depthStencilDesc,
		D3D12_RESOURCE_STATE_COMMON, &optClear, IID_PPV_ARGS(p_depthStencilBuffer.GetAddressOf()));
	if (FAILED(hr))
		DX3DError("SwapChain not created successfully.");

	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Format = p_system->p_depthStencilFormat;
	dsvDesc.Texture2D.MipSlice = 0;
	
	device->CreateDepthStencilView(p_depthStencilBuffer.Get(), &dsvDesc, p_system->p_descriptorHeap->depthStencilView());
	
	//D3D12_RESOURCE_BARRIER resBar;
	//ZeroMemory(&resBar, sizeof(resBar));
	//resBar.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	//resBar.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	//resBar.Transition.pResource = p_depthStencilBuffer.Get();
	//resBar.Transition.StateBefore = D3D12_RESOURCE_STATE_COMMON;
	//resBar.Transition.StateAfter = D3D12_RESOURCE_STATE_DEPTH_WRITE;
	//resBar.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	//p_system->p_commandMgr->p_commandList->ResourceBarrier(1, &resBar);
	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(p_depthStencilBuffer.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE);
	p_system->p_commandMgr->p_commandList->ResourceBarrier(1, &barrier);
	
	hr = p_system->p_commandMgr->p_commandList->Close();
	if (FAILED(hr))
		DX3DError("SwapChain not created successfully.");

	ID3D12CommandList* cmdsLists[] = { p_system->p_commandMgr->p_commandList.Get() };
	p_system->p_commandMgr->p_commandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	p_system->p_commandMgr->flushCommandQueue();

	p_screenViewport.TopLeftX = 0;
	p_screenViewport.TopLeftY = 0;
	p_screenViewport.Width = static_cast<float>(width);
	p_screenViewport.Height = static_cast<float>(height);
	p_screenViewport.MinDepth = 0.0f;
	p_screenViewport.MaxDepth = 1.0f;

	p_scissorRect = { 0, 0, static_cast<long>(width), static_cast<long>(height) };
}

ID3D12Resource* SwapChain::currentBackBuffer() const
{
	return p_swapChainBuffer[p_currBackBuffer].Get();
}
