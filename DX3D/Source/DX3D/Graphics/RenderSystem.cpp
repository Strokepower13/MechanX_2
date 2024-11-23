#include <DX3D/Graphics/RenderSystem.h>
#include <DX3D/Graphics/SwapChain.h>
#include <DX3D/Graphics/CommandManager.h>
#include <DX3D/Graphics/DescriptorHeap.h>
#include <DX3D/Graphics/VertexBuffer.h>
#include <DX3D/Graphics/IndexBuffer.h>
#include <DX3D/Graphics/ConstantBuffer.h>
#include <DX3D/Graphics/RootSignature.h>
#include <DX3D/Graphics/PipelineState.h>
#include <DX3D/Graphics/VertexShader.h>
//#include <DX3D/Graphics/GeometryShader.h>
#include <DX3D/Graphics/PixelShader.h>
//#include <DX3D/Resources/Texture.h>
//#include <DX3D/Resources/Texture2DArray.h>
//#include <DX3D/States/RasterizerState.h>
//#include <DX3D/States/BlendState.h>
//#include <DX3D/States/DepthStencilState.h>

#include <d3dcompiler.h>
#include <exception>

RenderSystem::RenderSystem()
{
    HRESULT hr = S_OK;

#if defined(DEBUG) || defined(_DEBUG) 
    // Enable the D3D12 debug layer.
    {
        Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
        hr = D3D12GetDebugInterface(IID_PPV_ARGS(&debugController));
        if (FAILED(hr))
            DX3DError("RenderSystem not created successfully.");

        debugController->EnableDebugLayer();
    }
#endif

    hr = CreateDXGIFactory1(IID_PPV_ARGS(&p_dxgiFactory));
    if (FAILED(hr))
        DX3DError("RenderSystem not created successfully.");

    HRESULT hardwareResult = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&p_d3dDevice));
    if (FAILED(hardwareResult))
    {
        Microsoft::WRL::ComPtr<IDXGIAdapter> p_warpAdapter;
        hr = p_dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&p_warpAdapter));
        if (FAILED(hr))
            DX3DError("RenderSystem not created successfully.");

        hr = D3D12CreateDevice(p_warpAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&p_d3dDevice));
        if (FAILED(hr))
            DX3DError("RenderSystem not created successfully.");
    }

    //Check 4xmsaa quality
    D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels{};
    msQualityLevels.Format = p_backBufferFormat;
    msQualityLevels.SampleCount = 4;
    msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
    msQualityLevels.NumQualityLevels = 0;

    hr = p_d3dDevice->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &msQualityLevels, sizeof(msQualityLevels));
    if (FAILED(hr))
        DX3DError("RenderSystem not created successfully.");

    p_4xMsaaQuality = msQualityLevels.NumQualityLevels;
    
#ifdef _DEBUG
    logAdapters();
#endif

    p_commandMgr = std::make_shared <CommandManager>(this);

    p_descriptorHeap = std::make_shared <DescriptorHeap>(this);

    p_rootSignature = std::make_shared <RootSignature>(this);
}

RenderSystem::~RenderSystem()
{
    if (p_d3dDevice != nullptr)
        p_commandMgr->flushCommandQueue();
}

SwapChainPtr RenderSystem::createSwapChain(HWND hwnd, UINT width, UINT height)
{
    return std::make_shared <SwapChain>(hwnd, width, height, this);
}

//SwapChainPtr RenderSystem::createSwapChain4xMsaa(HWND hwnd, UINT width, UINT height)
//{
//    return std::make_shared <SwapChain>(hwnd, width, height, this, true);
//}

CommandManagerPtr RenderSystem::getCommandMgr()
{
    return this->p_commandMgr;
}

VertexBufferPtr RenderSystem::createVertexBuffer(const void* data, UINT sizeVertex, UINT sizeList)
{
    return std::make_shared<VertexBuffer>(data, sizeVertex, sizeList, this);
}

IndexBufferPtr RenderSystem::createIndexBuffer(const void* data, UINT sizeVertex, UINT sizeList)
{
    return std::make_shared<IndexBuffer>(data, sizeVertex, sizeList, this);
}

ConstantBufferPtr RenderSystem::createConstantBuffer(UINT sizeData, UINT elementCount)
{
    return std::make_shared<ConstantBuffer>(sizeData, elementCount, this);
}

PipelineStatePtr RenderSystem::createPipelineState(const InputLayoutPtr& inputLayout, const VertexShaderPtr& vs, const PixelShaderPtr& ps)
{
    return std::make_shared<PipelineState>(inputLayout, vs, ps, this);
}

VertexShaderPtr RenderSystem::createVertexShader(const wchar_t* fullPath)
{
    return std::make_shared<VertexShader>(fullPath);
}

//GeometryShaderPtr RenderSystem::createGeometryShader(const wchar_t* fullPath, const char* entryPoint)
//{
//    return std::make_shared<GeometryShader>(fullPath, entryPoint, this);
//}

PixelShaderPtr RenderSystem::createPixelShader(const wchar_t* fullPath)
{
    return std::make_shared<PixelShader>(fullPath);
}
//
//TexturePtr RenderSystem::createTexture(const wchar_t* fullPath)
//{
//    return std::make_shared<Texture>(fullPath, this);
//}
//
//Texture2DArrayPtr RenderSystem::createTexture2DArray(std::vector<std::wstring>& filenames)
//{
//    return std::make_shared<Texture2DArray>(filenames, this);
//}
//
//VertexBufferPtr RenderSystem::createVertexBufferDynamic(UINT sizeVertex, UINT sizeList)
//{
//    return std::make_shared<VertexBuffer>(sizeVertex, sizeList, this);
//}
//
//void RenderSystem::clearState()
//{
//    p_d3dImmediateContext->ClearState();
//    //p_d3dImmediateContext->OMSetBlendState(p_alphaBlendState.Get(), 0, 0xffffffff);
//}
//
//RasterizerStatePtr RenderSystem::createRasterizerState(const FillMode& fillMode, const CullMode& cullMode, bool frontCounterClockwise)
//{
//    return std::make_shared<RasterizerState>(fillMode, cullMode, frontCounterClockwise, this);
//}
//
//BlendStatePtr RenderSystem::createBlendState(const BlendMode& blendMode)
//{
//    return std::make_shared<BlendState>(blendMode, this);
//}
//
//DepthStencilStatePtr RenderSystem::createDepthStencilState(const DepthStencilMode& dsMode)
//{
//    return std::make_shared<DepthStencilState>(dsMode, this);
//}

void RenderSystem::logAdapters()
{
    UINT i = 0;
    IDXGIAdapter* adapter = nullptr;
    std::vector<IDXGIAdapter*> adapterList;
    while (p_dxgiFactory->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND)
    {
        DXGI_ADAPTER_DESC desc;
        adapter->GetDesc(&desc);

        std::wstring text = L"***Adapter: ";
        text += desc.Description;
        text += L"\n";

        OutputDebugString(text.c_str());

        adapterList.push_back(adapter);

        ++i;
    }

    for (size_t i = 0; i < adapterList.size(); ++i)
    {
        logAdapterOutputs(adapterList[i]);
        adapterList[i]->Release();
        adapterList[i] = nullptr;
    }
}

void RenderSystem::logAdapterOutputs(IDXGIAdapter* adapter)
{
    UINT i = 0;
    IDXGIOutput* output = nullptr;
    while (adapter->EnumOutputs(i, &output) != DXGI_ERROR_NOT_FOUND)
    {
        DXGI_OUTPUT_DESC desc;
        output->GetDesc(&desc);

        std::wstring text = L"***Output: ";
        text += desc.DeviceName;
        text += L"\n";
        OutputDebugString(text.c_str());

        //logOutputDisplayModes(output, p_backBufferFormat);

        output->Release();
        output = nullptr;

        ++i;
    }
}

void RenderSystem::logOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format)
{
    UINT count = 0;
    UINT flags = 0;

    // Call with nullptr to get list count.
    output->GetDisplayModeList(format, flags, &count, nullptr);

    std::vector<DXGI_MODE_DESC> modeList(count);
    output->GetDisplayModeList(format, flags, &count, &modeList[0]);

    for (auto& x : modeList)
    {
        UINT n = x.RefreshRate.Numerator;
        UINT d = x.RefreshRate.Denominator;
        std::wstring text =
            L"Width = " + std::to_wstring(x.Width) + L" " +
            L"Height = " + std::to_wstring(x.Height) + L" " +
            L"Refresh = " + std::to_wstring(n) + L"/" + std::to_wstring(d) +
            L"\n";

        ::OutputDebugString(text.c_str());
    }
}
