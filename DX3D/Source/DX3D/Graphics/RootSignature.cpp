#include <DX3D/Graphics/RootSignature.h>
#include <DX3D/Graphics/RenderSystem.h>
#include <d3dx12.h>

RootSignature::RootSignature(const D3D12_ROOT_PARAMETER* rootParameter, UINT numParameters, RenderSystem* system) : p_system(system)
{
	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(numParameters, rootParameter, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	Microsoft::WRL::ComPtr<ID3DBlob> serializedRootSig = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

	if (errorBlob != nullptr)
		DX3DWarning("RootSignature warning: " << (char*)errorBlob->GetBufferPointer());
	if (FAILED(hr))
		DX3DError("RootSignature not created successfully.");

	hr = p_system->p_d3dDevice->CreateRootSignature(0, serializedRootSig->GetBufferPointer(), serializedRootSig->GetBufferSize(), IID_PPV_ARGS(&p_rootSignature));
	if (FAILED(hr))
		DX3DError("RootSignature not created successfully.");
}

RootSignature::~RootSignature()
{
}
