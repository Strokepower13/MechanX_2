#include <DX3D/Buffers/VertexBuffer.h>
#include <DX3D/Graphics/RenderSystem.h>
#include <DX3D/Graphics/CommandManager.h>
#include <d3dx12.h>

VertexBuffer::VertexBuffer(const void* data, UINT sizeVertex, UINT sizeList, RenderSystem* system) : p_system(system)
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

VertexBuffer::VertexBuffer(UINT sizeVertex, UINT sizeList, RenderSystem* system) : p_system(system)
{
	p_sizeVertex = sizeVertex;
	p_sizeList = sizeList;

	auto& device = p_system->p_d3dDevice;
	auto heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeVertex * static_cast<UINT64>(sizeList));

	HRESULT hr = device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE,
		&resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&p_buffer));
	if (FAILED(hr))
		DX3DError("VertexBuffer not created successfully.");

	hr = p_buffer->Map(0, nullptr, reinterpret_cast<void**>(&p_mappedData));
	if (FAILED(hr))
		DX3DError("VertexBuffer not created successfully.");
}

VertexBuffer::~VertexBuffer()
{
	if (p_buffer != nullptr && p_mappedData != nullptr)
		p_buffer->Unmap(0, nullptr);
	
	p_mappedData = nullptr;
}

void VertexBuffer::update(int elementIndex, const void* data)
{
	if (p_mappedData)
		memcpy(&p_mappedData[elementIndex * p_sizeVertex], data, p_sizeVertex);
	else
		DX3DWarning("VertexBuffer::update error. p_mappedData equals nullptr.");
}

D3D12_VERTEX_BUFFER_VIEW VertexBuffer::getVBV()
{
	D3D12_VERTEX_BUFFER_VIEW vbv{};
	vbv.BufferLocation = p_buffer->GetGPUVirtualAddress();
	vbv.StrideInBytes = p_sizeVertex;
	vbv.SizeInBytes = p_sizeVertex * p_sizeList;

	return vbv;
}
