#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <DX3D/Prerequisites.h>

class VertexBuffer
{
public:
	VertexBuffer(const void* data, UINT sizeVertex, UINT sizeList, RenderSystem* system);
	VertexBuffer(UINT sizeVertex, UINT sizeList, RenderSystem* system);
	~VertexBuffer();

	void update(int elementIndex, const void* data);

private:
	D3D12_VERTEX_BUFFER_VIEW getVBV();
	
	Microsoft::WRL::ComPtr<ID3D12Resource> p_buffer = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> p_uploadBuffer = nullptr;

	UINT p_sizeVertex;
	UINT p_sizeList;

	BYTE* p_mappedData = nullptr;

	RenderSystem* p_system = nullptr;
	friend class CommandManager;
};

