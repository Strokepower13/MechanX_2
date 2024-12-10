#include "Shapes.h"

Shapes::Shapes(HINSTANCE hInstance) : Game(hInstance)
{
	DirectX::XMMATRIX I = DirectX::XMMatrixIdentity();
	DirectX::XMStoreFloat4x4(&p_world, I);
	DirectX::XMStoreFloat4x4(&p_view, I);
	DirectX::XMStoreFloat4x4(&p_proj, I);
}

Shapes::~Shapes()
{
}

void Shapes::onCreate()
{
	auto commMgr = getGraphicsEngine()->getRenderSystem()->getCommandMgr();

	commMgr->resetCmdList();

	p_inputLayout = std::make_shared<InputLayout>(InputLayoutType::PosColor);

	auto rs = getGraphicsEngine()->getRenderSystem();

	CD3DX12_DESCRIPTOR_RANGE cbvTable0{};
	cbvTable0.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);

	CD3DX12_DESCRIPTOR_RANGE cbvTable1{};
	cbvTable1.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1);

	CD3DX12_ROOT_PARAMETER slotRootParameter[2]{};
	slotRootParameter[0].InitAsDescriptorTable(1, &cbvTable0);
	slotRootParameter[1].InitAsDescriptorTable(1, &cbvTable1);
	rs->createRootSignature(slotRootParameter, ARRAYSIZE(slotRootParameter));

	GeometryGenerator geoGen;
	GeometryGenerator::MeshData box = geoGen.createBox(1.5f, 0.5f, 1.5f, 3);
	GeometryGenerator::MeshData grid = geoGen.createGrid(20.0f, 30.0f, 60, 40);
	GeometryGenerator::MeshData sphere = geoGen.createSphere(0.5f, 20, 20);
	GeometryGenerator::MeshData cylinder = geoGen.createCylinder(0.5f, 0.3f, 3.0f, 20, 20);

	// Cache the vertex offsets to each object in the concatenated vertex buffer.
	UINT boxVertexOffset = 0;
	UINT gridVertexOffset = (UINT)box.vertices.size();
	UINT sphereVertexOffset = gridVertexOffset + (UINT)grid.vertices.size();
	UINT cylinderVertexOffset = sphereVertexOffset + (UINT)sphere.vertices.size();

	// Cache the starting index for each object in the concatenated index buffer.
	UINT boxIndexOffset = 0;
	UINT gridIndexOffset = (UINT)box.indices32.size();
	UINT sphereIndexOffset = gridIndexOffset + (UINT)grid.indices32.size();
	UINT cylinderIndexOffset = sphereIndexOffset + (UINT)sphere.indices32.size();

	SubMesh boxSM;
	boxSM.indexCount = (UINT)box.indices32.size();
	boxSM.startIndexLocation = boxIndexOffset;
	boxSM.baseVertexLocation = boxVertexOffset;

	SubMesh gridSM;
	gridSM.indexCount = (UINT)grid.indices32.size();
	gridSM.startIndexLocation = gridIndexOffset;
	gridSM.baseVertexLocation = gridVertexOffset;

	SubMesh sphereSM;
	sphereSM.indexCount = (UINT)sphere.indices32.size();
	sphereSM.startIndexLocation = sphereIndexOffset;
	sphereSM.baseVertexLocation = sphereVertexOffset;

	SubMesh cylinderSM;
	cylinderSM.indexCount = (UINT)cylinder.indices32.size();
	cylinderSM.startIndexLocation = cylinderIndexOffset;
	cylinderSM.baseVertexLocation = cylinderVertexOffset;

	auto totalVertexCount = box.vertices.size() + grid.vertices.size() + sphere.vertices.size() + cylinder.vertices.size();
	auto totalIndexCount = boxSM.indexCount + gridSM.indexCount + sphereSM.indexCount + cylinderSM.indexCount;

	std::vector<Vertex> vertices(totalVertexCount);

	UINT k = 0;
	for (size_t i = 0; i < box.vertices.size(); ++i, ++k)
	{
		vertices[k].pos = box.vertices[i].position;
		vertices[k].color = DirectX::XMFLOAT4(DirectX::Colors::DarkGreen);
	}

	for (size_t i = 0; i < grid.vertices.size(); ++i, ++k)
	{
		vertices[k].pos = grid.vertices[i].position;
		vertices[k].color = DirectX::XMFLOAT4(DirectX::Colors::ForestGreen);
	}

	for (size_t i = 0; i < sphere.vertices.size(); ++i, ++k)
	{
		vertices[k].pos = sphere.vertices[i].position;
		vertices[k].color = DirectX::XMFLOAT4(DirectX::Colors::Crimson);
	}

	for (size_t i = 0; i < cylinder.vertices.size(); ++i, ++k)
	{
		vertices[k].pos = cylinder.vertices[i].position;
		vertices[k].color = DirectX::XMFLOAT4(DirectX::Colors::SteelBlue);
	}

	std::vector<std::uint16_t> indices;
	indices.insert(indices.end(), std::begin(box.getIndices16()), std::end(box.getIndices16()));
	indices.insert(indices.end(), std::begin(grid.getIndices16()), std::end(grid.getIndices16()));
	indices.insert(indices.end(), std::begin(sphere.getIndices16()), std::end(sphere.getIndices16()));
	indices.insert(indices.end(), std::begin(cylinder.getIndices16()), std::end(cylinder.getIndices16()));

	p_mesh = std::make_unique<Mesh>();
	p_mesh->p_vertexBuffer = rs->createVertexBuffer(&vertices[0], sizeof(Vertex), (UINT)totalVertexCount);
	p_mesh->p_indexBuffer = rs->createIndexBuffer(&indices[0], sizeof(uint16_t), totalIndexCount);

	p_mesh->drawArgs["box"] = boxSM;
	p_mesh->drawArgs["grid"] = gridSM;
	p_mesh->drawArgs["sphere"] = sphereSM;
	p_mesh->drawArgs["cylinder"] = cylinderSM;


	auto boxRitem = std::make_unique<RenderItem>();
	DirectX::XMStoreFloat4x4(&boxRitem->world, DirectX::XMMatrixScaling(2.0f, 2.0f, 2.0f) * DirectX::XMMatrixTranslation(0.0f, 0.5f, 0.0f));
	boxRitem->objCBIndex = 0;
	boxRitem->mesh = p_mesh.get();
	boxRitem->primitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	boxRitem->indexCount = boxRitem->mesh->drawArgs["box"].indexCount;
	boxRitem->startIndexLocation = boxRitem->mesh->drawArgs["box"].startIndexLocation;
	boxRitem->baseVertexLocation = boxRitem->mesh->drawArgs["box"].baseVertexLocation;
	p_allRitems.push_back(std::move(boxRitem));

	auto gridRitem = std::make_unique<RenderItem>();
	DirectX::XMStoreFloat4x4(&gridRitem->world, DirectX::XMMatrixIdentity());
	gridRitem->objCBIndex = 1;
	gridRitem->mesh = p_mesh.get();
	gridRitem->primitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	gridRitem->indexCount = gridRitem->mesh->drawArgs["grid"].indexCount;
	gridRitem->startIndexLocation = gridRitem->mesh->drawArgs["grid"].startIndexLocation;
	gridRitem->baseVertexLocation = gridRitem->mesh->drawArgs["grid"].baseVertexLocation;
	p_allRitems.push_back(std::move(gridRitem));

	UINT objCBIndex = 2;
	for (int i = 0; i < 5; ++i)
	{
		auto leftCylRitem = std::make_unique<RenderItem>();
		auto rightCylRitem = std::make_unique<RenderItem>();
		auto leftSphereRitem = std::make_unique<RenderItem>();
		auto rightSphereRitem = std::make_unique<RenderItem>();

		DirectX::XMMATRIX leftCylWorld = DirectX::XMMatrixTranslation(-5.0f, 1.5f, -10.0f + i * 5.0f);
		DirectX::XMMATRIX rightCylWorld = DirectX::XMMatrixTranslation(+5.0f, 1.5f, -10.0f + i * 5.0f);

		DirectX::XMMATRIX leftSphereWorld = DirectX::XMMatrixTranslation(-5.0f, 3.5f, -10.0f + i * 5.0f);
		DirectX::XMMATRIX rightSphereWorld = DirectX::XMMatrixTranslation(+5.0f, 3.5f, -10.0f + i * 5.0f);

		DirectX::XMStoreFloat4x4(&leftCylRitem->world, leftCylWorld);
		leftCylRitem->objCBIndex = objCBIndex++;
		leftCylRitem->mesh = p_mesh.get();
		leftCylRitem->primitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		leftCylRitem->indexCount = leftCylRitem->mesh->drawArgs["cylinder"].indexCount;
		leftCylRitem->startIndexLocation = leftCylRitem->mesh->drawArgs["cylinder"].startIndexLocation;
		leftCylRitem->baseVertexLocation = leftCylRitem->mesh->drawArgs["cylinder"].baseVertexLocation;

		DirectX::XMStoreFloat4x4(&rightCylRitem->world, rightCylWorld);
		rightCylRitem->objCBIndex = objCBIndex++;
		rightCylRitem->mesh = p_mesh.get();
		rightCylRitem->primitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		rightCylRitem->indexCount = rightCylRitem->mesh->drawArgs["cylinder"].indexCount;
		rightCylRitem->startIndexLocation = rightCylRitem->mesh->drawArgs["cylinder"].startIndexLocation;
		rightCylRitem->baseVertexLocation = rightCylRitem->mesh->drawArgs["cylinder"].baseVertexLocation;

		DirectX::XMStoreFloat4x4(&leftSphereRitem->world, leftSphereWorld);
		leftSphereRitem->objCBIndex = objCBIndex++;
		leftSphereRitem->mesh = p_mesh.get();
		leftSphereRitem->primitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		leftSphereRitem->indexCount = leftSphereRitem->mesh->drawArgs["sphere"].indexCount;
		leftSphereRitem->startIndexLocation = leftSphereRitem->mesh->drawArgs["sphere"].startIndexLocation;
		leftSphereRitem->baseVertexLocation = leftSphereRitem->mesh->drawArgs["sphere"].baseVertexLocation;

		DirectX::XMStoreFloat4x4(&rightSphereRitem->world, rightSphereWorld);
		rightSphereRitem->objCBIndex = objCBIndex++;
		rightSphereRitem->mesh = p_mesh.get();
		rightSphereRitem->primitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		rightSphereRitem->indexCount = rightSphereRitem->mesh->drawArgs["sphere"].indexCount;
		rightSphereRitem->startIndexLocation = rightSphereRitem->mesh->drawArgs["sphere"].startIndexLocation;
		rightSphereRitem->baseVertexLocation = rightSphereRitem->mesh->drawArgs["sphere"].baseVertexLocation;

		p_allRitems.push_back(std::move(leftCylRitem));
		p_allRitems.push_back(std::move(rightCylRitem));
		p_allRitems.push_back(std::move(leftSphereRitem));
		p_allRitems.push_back(std::move(rightSphereRitem));
	}

	// All the render items are opaque.
	for (auto& e : p_allRitems)
		p_opaqueRitems.push_back(e.get());

	UINT objCount = (UINT)p_opaqueRitems.size();
	p_passCbvOffset = objCount * gNumFrameResources;

	rs->createCbvDescriptorHeap((objCount + 1) * gNumFrameResources);

	for (int i = 0; i < gNumFrameResources; ++i)
	{
		p_frameResources.push_back(std::make_unique<FrameResource>(1, (UINT)p_allRitems.size(), i, p_passCbvOffset, rs));
	}

	p_vs = rs->createVertexShader(L"Assets/Shaders/ColorVS.cso");
	p_ps = rs->createPixelShader(L"Assets/Shaders/ColorPS.cso");

	p_pso = rs->createPipelineState(p_inputLayout, p_vs, p_ps);

	commMgr->closeCmdList();

	commMgr->setPSO(p_pso);
}

void Shapes::onResize()
{
	auto cmd = getGraphicsEngine()->getRenderSystem()->getCommandMgr();

	cmd->begin();
	cmd->finish();
	
	auto AR = getDisplay()->getAspectRatio();
	DirectX::XMMATRIX P = DirectX::XMMatrixPerspectiveFovLH(0.25f * DirectX::XM_PI, AR, 1.0f, 1000.0f);
	DirectX::XMStoreFloat4x4(&p_proj, P);
}

void Shapes::onUpdate(float deltaTime)
{
	auto scptr = getDisplay()->getSwapChain();
	auto cmdMgr = getGraphicsEngine()->getRenderSystem()->getCommandMgr();

	if (getInputSystem()->isLButton())
	{
		DirectX::XMFLOAT2 deltaMouse = getInputSystem()->getDeltaMousePosition();
		float dx = DirectX::XMConvertToRadians(0.25f * deltaMouse.x);
		float dy = DirectX::XMConvertToRadians(0.25f * deltaMouse.y);
		p_theta += dx;
		p_phi += dy;
		p_phi = max(0.1f, min(p_phi, DirectX::XM_PI - 0.1f));
	}

	if (getInputSystem()->isRButton())
	{
		DirectX::XMFLOAT2 deltaMouse = getInputSystem()->getDeltaMousePosition();
		float dx = 0.005f * deltaMouse.x;
		float dy = 0.005f * deltaMouse.y;

		p_radius += dx - dy;
		p_radius = max(3.0f, min(p_radius, 15.0f));
	}

	if (GetAsyncKeyState('Y') & 0x8000)
	{
		getGraphicsEngine()->getRenderSystem()->setMSAAState(true);
		p_pso.reset();
		p_pso = getGraphicsEngine()->getRenderSystem()->createPipelineState(p_inputLayout, p_vs, p_ps);
		//cmdMgr->resetCmdList();
		cmdMgr->setPSO(p_pso);
		//getDisplay()->onSize();
	}

	if (GetAsyncKeyState('N') & 0x8000)
	{
		getGraphicsEngine()->getRenderSystem()->setMSAAState(false);
		p_pso.reset();
		p_pso = getGraphicsEngine()->getRenderSystem()->createPipelineState(p_inputLayout, p_vs, p_ps);
		cmdMgr->setPSO(p_pso);
	}

	ObjectConstants cb{};
	p_eyePos.x = p_radius * sinf(p_phi) * cosf(p_theta);
	p_eyePos.z = p_radius * sinf(p_phi) * sinf(p_theta);
	p_eyePos.y = p_radius * cosf(p_phi);

	DirectX::XMVECTOR pos = DirectX::XMVectorSet(p_eyePos.x, p_eyePos.y, p_eyePos.z, 1.0f);
	DirectX::XMVECTOR target = DirectX::XMVectorZero();
	DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	DirectX::XMMATRIX V = DirectX::XMMatrixLookAtLH(pos, target, up);
	DirectX::XMStoreFloat4x4(&p_view, V);

	p_currFrameResourceIndex = (p_currFrameResourceIndex + 1) % gNumFrameResources;
	p_currFrameResource = p_frameResources[p_currFrameResourceIndex].get();

	cmdMgr->setFence(p_currFrameResource->p_fence);

	auto& currObjectCB = p_currFrameResource->p_objectCB;
	for (auto& e : p_allRitems)
	{
		// Only update the cbuffer data if the constants have changed.  
		// This needs to be tracked per frame resource.
		if (e->numFramesDirty > 0)
		{
			DirectX::XMMATRIX world = DirectX::XMLoadFloat4x4(&e->world);

			ObjectConstants objConstants;
			XMStoreFloat4x4(&objConstants.World, world);

			currObjectCB->update(e->objCBIndex, &objConstants);

			// Next FrameResource need to be updated too.
			e->numFramesDirty--;
		}
	}

	DirectX::XMMATRIX view = DirectX::XMLoadFloat4x4(&p_view);
	DirectX::XMMATRIX proj = DirectX::XMLoadFloat4x4(&p_proj);

	DirectX::XMMATRIX viewProj = DirectX::XMMatrixMultiply(view, proj);
	auto det = DirectX::XMMatrixDeterminant(view);
	DirectX::XMMATRIX invView = DirectX::XMMatrixInverse(&det, view);
	det = DirectX::XMMatrixDeterminant(proj);
	DirectX::XMMATRIX invProj = DirectX::XMMatrixInverse(&det, proj);
	det = DirectX::XMMatrixDeterminant(viewProj);
	DirectX::XMMATRIX invViewProj = DirectX::XMMatrixInverse(&det, viewProj);

	auto clientWidth = getDisplay()->getClientWidth();
	auto clientHeight = getDisplay()->getClientHeight();

	DirectX::XMStoreFloat4x4(&p_mainPassCB.View, view);
	DirectX::XMStoreFloat4x4(&p_mainPassCB.InvView, invView);
	DirectX::XMStoreFloat4x4(&p_mainPassCB.Proj, proj);
	DirectX::XMStoreFloat4x4(&p_mainPassCB.InvProj, invProj);
	DirectX::XMStoreFloat4x4(&p_mainPassCB.ViewProj, viewProj);
	DirectX::XMStoreFloat4x4(&p_mainPassCB.InvViewProj, invViewProj);
	p_mainPassCB.EyePosW = p_eyePos;
	p_mainPassCB.RenderTargetSize = DirectX::XMFLOAT2((float)clientWidth, (float)clientHeight);
	p_mainPassCB.InvRenderTargetSize = DirectX::XMFLOAT2(1.0f / clientWidth, 1.0f / clientHeight);
	p_mainPassCB.NearZ = 1.0f;
	p_mainPassCB.FarZ = 1000.0f;
	p_mainPassCB.TotalTime = getTimer()->totalTime();
	p_mainPassCB.DeltaTime = deltaTime;

	auto& currPassCB = p_currFrameResource->p_passCB;
	currPassCB->update(0, &p_mainPassCB);

	auto& cmdListAlloc = p_currFrameResource->p_cmdListAlloc;
	HRESULT hr = cmdListAlloc->Reset();

	//Begin drawing
	cmdMgr->begin(cmdListAlloc.Get());
	cmdMgr->setViewportSize();
	cmdMgr->clearRenderTargetColor(DirectX::Colors::LightSteelBlue);

	cmdMgr->setDescriptorHeaps();
	cmdMgr->setRootSignature();

	int passCbvIndex = p_passCbvOffset + p_currFrameResourceIndex;
	
	cmdMgr->setDescriptorTable(1, passCbvIndex);

	// For each render item...
	for (size_t i = 0; i < p_opaqueRitems.size(); ++i)
	{
		auto ri = p_opaqueRitems[i];

		cmdMgr->setVertexBuffer(ri->mesh->p_vertexBuffer);
		cmdMgr->setIndexBuffer(ri->mesh->p_indexBuffer);

		UINT cbvIndex = p_currFrameResourceIndex * (UINT)p_opaqueRitems.size() + ri->objCBIndex;
		
		cmdMgr->setDescriptorTable(0, cbvIndex);

		cmdMgr->drawIndexedTriangleList(ri->indexCount, ri->startIndexLocation, ri->baseVertexLocation);
	}

	cmdMgr->finish();

	scptr->present(true);

	cmdMgr->signal(p_currFrameResource->p_fence);
}
