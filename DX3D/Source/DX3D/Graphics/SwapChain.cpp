#include <DX3D/Graphics/SwapChain.h>
#include <DX3D/Graphics/RenderSystem.h>
#include <DX3D/Graphics/CommandManager.h>
#include <DX3D/Graphics/DescriptorHeap.h>
#include <exception>
#include <sstream>
#include <d3dx12.h>

SwapChain::SwapChain(HWND hwnd, UINT width, UINT height, RenderSystem* system) :p_system(system)
{
	p_swapChain.Reset();

	p_swapChainBufferCount = p_system->p_swapChainBufferCount;
	p_swapChainBuffer.resize(p_swapChainBufferCount);

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferDesc.Width = (width > 1) ? width : 1;
	sd.BufferDesc.Height = (height > 1) ? height : 1;
	sd.BufferDesc.RefreshRate.Numerator = 165;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = p_system->p_backBufferFormat;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
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

	commandMgr->flushCommandQueue();

	HRESULT hr = commandMgr->p_commandList->Reset(commandMgr->p_directCmdListAlloc.Get(), nullptr);
	if (FAILED(hr))
		DX3DError("SwapChain not created successfully.");

	for (int i = 0; i < p_swapChainBufferCount; ++i)
		p_swapChainBuffer[i].Reset();
	p_depthStencilBuffer.Reset();

	hr = p_swapChain->ResizeBuffers(p_swapChainBufferCount, width, height, p_system->p_backBufferFormat, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
	if (FAILED(hr))
		DX3DError("SwapChain not created successfully.");

	reloadBuffers(width, height);
}

void SwapChain::present(bool vsync, bool flush)
{
	HRESULT hr = p_swapChain->Present(vsync, NULL);
	if (FAILED(hr))
		DX3DError("SwapChain::present error.");

	p_currBackBuffer = (p_currBackBuffer + 1) % p_swapChainBufferCount;

	if (flush)
		p_system->p_commandMgr->flushCommandQueue();
}

void SwapChain::reloadBuffers(unsigned int width, unsigned int height)
{
	auto& commandMgr = p_system->p_commandMgr;

	auto& device = p_system->p_d3dDevice;

	p_currBackBuffer = 0;

	HRESULT hr = S_OK;

	auto& descriptorHeap = p_system->p_descriptorHeap;
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(descriptorHeap->p_rtvHeap->GetCPUDescriptorHandleForHeapStart());
	for (int i = 0; i < p_swapChainBufferCount; i++)
	{
		hr = p_swapChain->GetBuffer(i, IID_PPV_ARGS(&p_swapChainBuffer[i]));
		if (FAILED(hr))
			DX3DError("SwapChain not created successfully.");

		p_system->p_d3dDevice->CreateRenderTargetView(p_swapChainBuffer[i].Get(), nullptr, rtvHeapHandle);
		rtvHeapHandle.Offset(1, descriptorHeap->p_rtvDescriptorSize);
	}

	D3D12_RESOURCE_DESC depthStencilDesc{};
	depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthStencilDesc.Alignment = 0;
	depthStencilDesc.Width = width;
	depthStencilDesc.Height = height;
	depthStencilDesc.DepthOrArraySize = 1;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE optClear{};
	optClear.Format = p_system->p_depthStencilFormat;
	optClear.DepthStencil.Depth = 1.0f;
	optClear.DepthStencil.Stencil = 0;

	auto heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	hr = device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &depthStencilDesc,
		D3D12_RESOURCE_STATE_COMMON, &optClear, IID_PPV_ARGS(p_depthStencilBuffer.GetAddressOf()));
	if (FAILED(hr))
		DX3DError("SwapChain not created successfully.");

	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Format = p_system->p_depthStencilFormat;
	dsvDesc.Texture2D.MipSlice = 0;
	
	device->CreateDepthStencilView(p_depthStencilBuffer.Get(), &dsvDesc, descriptorHeap->depthStencilView());

	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(p_depthStencilBuffer.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE);
	commandMgr->p_commandList->ResourceBarrier(1, &barrier);
	
	hr = commandMgr->p_commandList->Close();
	if (FAILED(hr))
		DX3DError("SwapChain not created successfully.");

	ID3D12CommandList* cmdsLists[] = { commandMgr->p_commandList.Get() };
	commandMgr->p_commandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	commandMgr->flushCommandQueue();

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
