#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <DX3D/Prerequisites.h>

class MSAAResources
{
public:
	MSAAResources(UINT width, UINT height, RenderSystem* system);
	~MSAAResources();
	void resize(UINT width, UINT height);

private:
	RenderSystem* p_system = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> p_renderTarget = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> p_depthStencil = nullptr;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> p_RTVDescriptorHeap;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> p_DSVDescriptorHeap;

	friend class CommandManager;
};
