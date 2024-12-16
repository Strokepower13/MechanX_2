#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <DX3D/Prerequisites.h>
#include <DirectXMath.h>

struct ObjectConstants
{
    DirectX::XMFLOAT4X4 world{};
};

struct PassConstants
{
    DirectX::XMFLOAT4X4 view{};
    DirectX::XMFLOAT4X4 invView{};
    DirectX::XMFLOAT4X4 proj{};
    DirectX::XMFLOAT4X4 invProj{};
    DirectX::XMFLOAT4X4 viewProj{};
    DirectX::XMFLOAT4X4 invViewProj{};
    DirectX::XMFLOAT3 eyePosW = { 0.0f, 0.0f, 0.0f };
    float cbPerObjectPad1 = 0.0f;
    DirectX::XMFLOAT2 renderTargetSize = { 0.0f, 0.0f };
    DirectX::XMFLOAT2 invRenderTargetSize = { 0.0f, 0.0f };
    float nearZ = 0.0f;
    float farZ = 0.0f;
    float totalTime = 0.0f;
    float deltaTime = 0.0f;
};

struct Vertex
{
    DirectX::XMFLOAT3 pos;
    DirectX::XMFLOAT4 color;
};

class FrameResource
{
public:
    FrameResource(UINT passCount, UINT objectCount, UINT numResource, UINT passCbvOffset, RenderSystem* system);
    FrameResource(UINT passCount, UINT objectCount, UINT waveVertCount, RenderSystem* system);

public:
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> p_cmdListAlloc;

    RenderSystem* p_system = nullptr;

    ConstantBufferPtr p_passCB = nullptr;
    ConstantBufferPtr p_objectCB = nullptr;
    VertexBufferPtr p_wavesVB = nullptr;

    UINT64 p_fence = 0;
};

