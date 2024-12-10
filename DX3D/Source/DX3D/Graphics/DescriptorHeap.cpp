#include <DX3D/Graphics/DescriptorHeap.h>
#include <DX3D/Graphics/RenderSystem.h>
#include <DX3D/Graphics/SwapChain.h>
#include <d3dx12.h>

DescriptorHeap::DescriptorHeap(RenderSystem* system) : p_system(system)
{
    auto& device = p_system->p_d3dDevice;

    p_rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    p_dsvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
    p_cbvSrvUavDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};
    rtvHeapDesc.NumDescriptors = p_system->p_swapChainBufferCount;
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    rtvHeapDesc.NodeMask = 0;

    HRESULT hr = device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(p_rtvHeap.GetAddressOf()));
    if (FAILED(hr))
        DX3DError("DescriptorHeap not created successfully.");


    D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
    dsvHeapDesc.NumDescriptors = 1;
    dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    dsvHeapDesc.NodeMask = 0;

    hr = device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(p_dsvHeap.GetAddressOf()));
    if (FAILED(hr))
        DX3DError("DescriptorHeap not created successfully.");
}

DescriptorHeap::~DescriptorHeap()
{
}

D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeap::depthStencilView() const
{
    return p_dsvHeap->GetCPUDescriptorHandleForHeapStart();
}

D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeap::currentBackBufferView(const SwapChainPtr& swapChain) const
{
    return CD3DX12_CPU_DESCRIPTOR_HANDLE(p_rtvHeap->GetCPUDescriptorHandleForHeapStart(), swapChain->p_currBackBuffer, p_rtvDescriptorSize);
}

void DescriptorHeap::createCbvDescriptorHeap(UINT numDescriptors)
{
    D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc{};
    cbvHeapDesc.NumDescriptors = numDescriptors;
    cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    cbvHeapDesc.NodeMask = 0;

    HRESULT hr = p_system->p_d3dDevice->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(&p_cbvHeap));
    if (FAILED(hr))
        DX3DError("DescriptorHeap::createCbvDescriptorHeap error.");
}
