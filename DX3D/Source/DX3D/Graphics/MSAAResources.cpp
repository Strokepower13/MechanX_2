#include <DX3D/Graphics/MSAAResources.h>
#include <DX3D/Graphics/RenderSystem.h>
#include <d3dx12.h>
#include <DirectXColors.h>

MSAAResources::MSAAResources(UINT width, UINT height, RenderSystem* system) : p_system(system)
{
	auto& device = p_system->p_d3dDevice;

	D3D12_DESCRIPTOR_HEAP_DESC rtvDescriptorHeapDesc = {};
	rtvDescriptorHeapDesc.NumDescriptors = 1;
	rtvDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

	HRESULT hr = device->CreateDescriptorHeap(&rtvDescriptorHeapDesc,
		IID_PPV_ARGS(p_RTVDescriptorHeap.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		DX3DError("MSAA not created successfully.");

	D3D12_DESCRIPTOR_HEAP_DESC dsvDescriptorHeapDesc = {};
	dsvDescriptorHeapDesc.NumDescriptors = 1;
	dsvDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;

	hr = device->CreateDescriptorHeap(&dsvDescriptorHeapDesc,
		IID_PPV_ARGS(p_DSVDescriptorHeap.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		DX3DError("MSAA not created successfully.");

	this->resize(width, height);
}

MSAAResources::~MSAAResources()
{

}

void MSAAResources::resize(UINT width, UINT height)
{
	auto& device = p_system->p_d3dDevice;
	
	p_renderTarget.Reset();
	p_depthStencil.Reset();

	CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_DEFAULT);

	D3D12_RESOURCE_DESC rtDesc = CD3DX12_RESOURCE_DESC::Tex2D(p_system->p_backBufferFormat, width, height, 1, 1, 4);
	rtDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

	D3D12_CLEAR_VALUE clearValue{};
	clearValue.Format = p_system->p_backBufferFormat;
	memcpy(clearValue.Color, DirectX::Colors::LightSteelBlue, sizeof(float) * 4);

	HRESULT hr = device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &rtDesc, D3D12_RESOURCE_STATE_RESOLVE_SOURCE, &clearValue, IID_PPV_ARGS(p_renderTarget.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		DX3DError("MSAA not created successfully.");

	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	rtvDesc.Format = p_system->p_backBufferFormat;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DMS;

	device->CreateRenderTargetView(p_renderTarget.Get(), &rtvDesc, p_RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

	D3D12_RESOURCE_DESC depthStencilDesc = CD3DX12_RESOURCE_DESC::Tex2D(p_system->p_depthStencilFormat, width, height, 1, 1, 4);
	depthStencilDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.Format = p_system->p_depthStencilFormat;
	depthClearValue.DepthStencil.Depth = 1.0f;
	depthClearValue.DepthStencil.Stencil = 0;

	hr = device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &depthStencilDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&depthClearValue, IID_PPV_ARGS(p_depthStencil.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		DX3DError("MSAA not created successfully.");

	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = p_system->p_depthStencilFormat;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DMS;

	device->CreateDepthStencilView(p_depthStencil.Get(), &dsvDesc, p_DSVDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
}
