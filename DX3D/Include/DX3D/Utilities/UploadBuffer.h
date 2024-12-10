#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <DX3D/Prerequisites.h>
#include <d3dx12.h>

template<typename T>
class UploadBuffer
{
public:
	UploadBuffer(ID3D12Device* device, UINT elementCount, bool isConstantBuffer) :p_isConstantBuffer(isConstantBuffer)
	{
		p_elementByteSize = sizeof(T);

		if (isConstantBuffer)
			p_elementByteSize = (sizeof(T) + 255) & ~255;

		auto heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(p_elementByteSize * static_cast<UINT64>(elementCount));

		HRESULT hr = device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE,
			&resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&p_uploadBuffer));
		if (FAILED(hr))
			DX3DError("UploadBuffer: constructor error.");

		hr = p_uploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&p_mappedData));
		if (FAILED(hr))
			DX3DError("UploadBuffer: constructor error.");
	}

	UploadBuffer(const UploadBuffer& rhs) = delete;
	UploadBuffer& operator=(const UploadBuffer& rhs) = delete;
	~UploadBuffer()
	{
		if (p_uploadBuffer != nullptr)
			p_uploadBuffer->Unmap(0, nullptr);

		p_mappedData = nullptr;
	}

	ID3D12Resource* resource() const
	{
		return p_uploadBuffer.Get();
	}

	void copyData(int elementIndex, const T& data)
	{
		memcpy(&p_mappedData[elementIndex * p_elementByteSize], &data, sizeof(T));
	}

private:
	Microsoft::WRL::ComPtr<ID3D12Resource> p_uploadBuffer;
	BYTE* p_mappedData = nullptr;

	UINT p_elementByteSize = 0;
	bool p_isConstantBuffer = false;
};