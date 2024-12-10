#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <DX3D/Prerequisites.h>
#include <DirectXMath.h>

struct ObjectConstants
{
    DirectX::XMFLOAT4X4 World{};
};

struct PassConstants
{
    DirectX::XMFLOAT4X4 View{};
    DirectX::XMFLOAT4X4 InvView{};
    DirectX::XMFLOAT4X4 Proj{};
    DirectX::XMFLOAT4X4 InvProj{};
    DirectX::XMFLOAT4X4 ViewProj{};
    DirectX::XMFLOAT4X4 InvViewProj{};
    DirectX::XMFLOAT3 EyePosW = { 0.0f, 0.0f, 0.0f };
    float cbPerObjectPad1 = 0.0f;
    DirectX::XMFLOAT2 RenderTargetSize = { 0.0f, 0.0f };
    DirectX::XMFLOAT2 InvRenderTargetSize = { 0.0f, 0.0f };
    float NearZ = 0.0f;
    float FarZ = 0.0f;
    float TotalTime = 0.0f;
    float DeltaTime = 0.0f;
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

public:
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> p_cmdListAlloc;

    RenderSystem* p_system = nullptr;

    ConstantBufferPtr p_passCB = nullptr;
    ConstantBufferPtr p_objectCB = nullptr;

    UINT64 p_fence = 0;
};

