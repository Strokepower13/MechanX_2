#include <DX3D/Resources/FrameResource.h>
#include <DX3D/Graphics/RenderSystem.h>
#include <DX3D/Buffers/ConstantBuffer.h>

FrameResource::FrameResource(UINT passCount, UINT objectCount, UINT numResource, UINT passCbvOffset, RenderSystem* system) :p_system(system)
{
	HRESULT hr = p_system->p_d3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(p_cmdListAlloc.GetAddressOf()));
	if (FAILED(hr))
		DX3DError("FrameResource not created successfully.");

	p_objectCB = p_system->createConstantBuffer(sizeof(ObjectConstants), objectCount, true);
	p_objectCB->createCbvForSomeObjects(numResource, objectCount);
	
	p_passCB = p_system->createConstantBuffer(sizeof(PassConstants), passCount, true);
	p_passCB->createCbv(numResource, passCbvOffset);
}

FrameResource::FrameResource(UINT passCount, UINT objectCount, UINT waveVertCount, RenderSystem* system) :p_system(system)
{
	HRESULT hr = p_system->p_d3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(p_cmdListAlloc.GetAddressOf()));
	if (FAILED(hr))
		DX3DError("FrameResource not created successfully.");

	p_objectCB = p_system->createConstantBuffer(sizeof(ObjectConstants), objectCount, true);
	p_passCB = p_system->createConstantBuffer(sizeof(PassConstants), passCount, true);
	p_wavesVB = p_system->createVertexBuffer(nullptr, sizeof(Vertex), waveVertCount, true);
}
