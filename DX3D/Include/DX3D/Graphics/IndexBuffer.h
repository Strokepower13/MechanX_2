#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <DX3D/Prerequisites.h>

class IndexBuffer
{
public:
	IndexBuffer(const void* data, UINT sizeVertex, UINT sizeList, RenderSystem* system);
	~IndexBuffer();

private:
	D3D12_INDEX_BUFFER_VIEW getIBV();
	
	Microsoft::WRL::ComPtr<ID3D12Resource> p_buffer = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> p_uploadBuffer = nullptr;

	UINT p_sizeVertex;
	UINT p_sizeList;

	RenderSystem* p_system = nullptr;
	friend class CommandManager;
};

