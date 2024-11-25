#include <DX3D/Graphics/IndexBuffer.h>
#include <DX3D/Graphics/RenderSystem.h>
#include <DX3D/Graphics/CommandManager.h>
#include <d3dx12.h>

IndexBuffer::IndexBuffer(const void* data, UINT sizeVertex, UINT sizeList, RenderSystem* system) : p_system(system)
{
	p_sizeVertex = sizeVertex;
	p_sizeList = sizeList;
	
	auto& device = p_system->p_d3dDevice;
	auto heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(static_cast<UINT64>(sizeVertex) * sizeList);

	HRESULT hr = device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE,
		&resourceDesc, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(p_buffer.GetAddressOf()));
	if (FAILED(hr))
		DX3DError("VertexBuffer not created successfully.");

	heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

	hr = device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE,
		&resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(p_uploadBuffer.GetAddressOf()));
	if (FAILED(hr))
		DX3DError("VertexBuffer not created successfully.");

	D3D12_SUBRESOURCE_DATA subResourceData{};
	subResourceData.pData = data;
	subResourceData.RowPitch = static_cast<LONG_PTR>(sizeVertex) * sizeList;
	subResourceData.SlicePitch = subResourceData.RowPitch;

	auto& cmdList = p_system->p_commandMgr->p_commandList;

	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(p_buffer.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST);
	cmdList->ResourceBarrier(1, &barrier);
	UpdateSubresources<1>(cmdList.Get(), p_buffer.Get(), p_uploadBuffer.Get(), 0, 0, 1, &subResourceData);
	auto barrier2 = CD3DX12_RESOURCE_BARRIER::Transition(p_buffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ);
	cmdList->ResourceBarrier(1, &barrier2);
}

IndexBuffer::~IndexBuffer()
{
}

D3D12_INDEX_BUFFER_VIEW IndexBuffer::getIBV()
{
	D3D12_INDEX_BUFFER_VIEW ibv{};
	ibv.BufferLocation = p_buffer->GetGPUVirtualAddress();
	ibv.Format = DXGI_FORMAT_R16_UINT;
	ibv.SizeInBytes = p_sizeVertex * p_sizeList;

	return ibv;
}
