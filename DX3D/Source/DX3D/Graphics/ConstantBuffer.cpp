#include <DX3D/Graphics/ConstantBuffer.h>
#include <DX3D/Graphics/RenderSystem.h>
#include <DX3D/Graphics/DescriptorHeap.h>
#include <d3dx12.h>

ConstantBuffer::ConstantBuffer(UINT sizeData, UINT elementCount, RenderSystem* system) : p_system(system)
{
	p_sizeData = sizeData;
	p_elementByteSize = (sizeData + 255) & ~255;

	auto& device = p_system->p_d3dDevice;
	auto heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(p_elementByteSize * static_cast<UINT64>(elementCount));

	HRESULT hr = device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE,
		&resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&p_buffer));
	if (FAILED(hr))
		DX3DError("ConstantBuffer not created successfully.");

	hr = p_buffer->Map(0, nullptr, reinterpret_cast<void**>(&p_mappedData));
	if (FAILED(hr))
		DX3DError("ConstantBuffer not created successfully.");

	D3D12_GPU_VIRTUAL_ADDRESS cbAdress = p_buffer->GetGPUVirtualAddress();

	int boxCBufIndex = 0;

	cbAdress += (UINT64)boxCBufIndex * (UINT64)p_elementByteSize;

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{};
	cbvDesc.BufferLocation = cbAdress;
	cbvDesc.SizeInBytes = p_elementByteSize;

	auto& descriptorHeap = p_system->p_descriptorHeap;
	device->CreateConstantBufferView(&cbvDesc, descriptorHeap->p_cbvHeap->GetCPUDescriptorHandleForHeapStart());
}

ConstantBuffer::~ConstantBuffer()
{
	if (p_buffer != nullptr)
		p_buffer->Unmap(0, nullptr);

	p_mappedData = nullptr;
}

void ConstantBuffer::update(int elementIndex, const void* data)
{
	memcpy(&p_mappedData[elementIndex * p_elementByteSize], data, p_sizeData);
}
