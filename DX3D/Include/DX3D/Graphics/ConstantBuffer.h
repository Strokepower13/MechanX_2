#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <DX3D/Prerequisites.h>

class ConstantBuffer
{
public:
	ConstantBuffer(UINT sizeData, UINT elementCount, bool withoutCBV, RenderSystem* system);
	~ConstantBuffer();

	void update(int elementIndex, const void* data);
	void createCbvForSomeObjects(UINT numResource, UINT objectsCount);
	void createCbv(UINT numResource, UINT cbvOffset);

private:
	Microsoft::WRL::ComPtr<ID3D12Resource> p_buffer;
	RenderSystem* p_system;
	BYTE* p_mappedData = nullptr;

	UINT p_elementByteSize = 0;
	UINT p_sizeData = 0;
};