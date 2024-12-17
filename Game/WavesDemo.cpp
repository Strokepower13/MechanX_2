#include "WavesDemo.h"

WavesDemo::WavesDemo(HINSTANCE hInstance) : Game(hInstance)
{
	DirectX::XMMATRIX I = DirectX::XMMatrixIdentity();
	DirectX::XMStoreFloat4x4(&p_view, I);
	DirectX::XMStoreFloat4x4(&p_proj, I);

	p_waves = std::make_unique<Waves>(128, 128, 1.0f, 0.03f, 4.0f, 0.2f);
}

WavesDemo::~WavesDemo()
{
	getGraphicsEngine()->getRenderSystem()->getCommandMgr()->flushCommandQueue();
}

void WavesDemo::onCreate()
{
	auto cmd = getGraphicsEngine()->getRenderSystem()->getCommandMgr();

	cmd->resetCmdList();

	auto rs = getGraphicsEngine()->getRenderSystem();

	p_inputLayout = rs->createInputLayout(InputLayoutType::PosColor);

	CD3DX12_ROOT_PARAMETER slotRootParameter[2]{};
	slotRootParameter[0].InitAsConstantBufferView(0);
	slotRootParameter[1].InitAsConstantBufferView(1);

	rs->createRootSignature(slotRootParameter, ARRAYSIZE(slotRootParameter));

	p_VSs["standardVS"] = rs->createVertexShader(L"Assets/Shaders/ColorVS.cso");
	p_PSs["opaquePS"] = rs->createPixelShader(L"Assets/Shaders/ColorPS.cso");


	//Land geometry
	GeometryGenerator geoGen;
	GeometryGenerator::MeshData grid = geoGen.createGrid(160.0f, 160.0f, 50, 50);

	std::vector<Vertex> vertices(grid.vertices.size());
	for (size_t i = 0; i < grid.vertices.size(); ++i)
	{
		auto& p = grid.vertices[i].position;
		vertices[i].pos = p;
		vertices[i].pos.y = getHillsHeight(p.x, p.z);

		if (vertices[i].pos.y < -10.0f)
		{
			// Sandy beach color.
			vertices[i].color = DirectX::XMFLOAT4(1.0f, 0.96f, 0.62f, 1.0f);
		}
		else if (vertices[i].pos.y < 5.0f)
		{
			// Light yellow-green.
			vertices[i].color = DirectX::XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
		}
		else if (vertices[i].pos.y < 12.0f)
		{
			// Dark yellow-green.
			vertices[i].color = DirectX::XMFLOAT4(0.1f, 0.48f, 0.19f, 1.0f);
		}
		else if (vertices[i].pos.y < 20.0f)
		{
			// Dark brown.
			vertices[i].color = DirectX::XMFLOAT4(0.45f, 0.39f, 0.34f, 1.0f);
		}
		else
		{
			// White snow.
			vertices[i].color = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		}
	}

	std::vector<std::uint16_t> indices = grid.getIndices16();

	auto mesh = std::make_unique<Mesh>();
	mesh->name = "land";

	mesh->p_vertexBuffer = rs->createVertexBuffer(&vertices[0], sizeof(Vertex), (UINT)vertices.size());
	mesh->p_indexBuffer = rs->createIndexBuffer(&indices[0], sizeof(uint16_t), (UINT)indices.size());

	SubMesh submesh;
	submesh.indexCount = (UINT)indices.size();
	submesh.startIndexLocation = 0;
	submesh.baseVertexLocation = 0;

	mesh->drawArgs["grid"] = submesh;

	p_meshes["land"] = std::move(mesh);

	//Waves geometry
	indices.resize((size_t)3 * p_waves->triangleCount());

	int m = p_waves->rowCount();
	int n = p_waves->columnCount();
	int k = 0;

	for (int i = 0; i < m - 1; ++i)
	{
		for (int j = 0; j < n - 1; ++j)
		{
			indices[k] = i * n + j;
			indices[(size_t)k + 1] = i * n + j + 1;
			indices[(size_t)k + 2] = (i + 1) * n + j;

			indices[(size_t)k + 3] = (i + 1) * n + j;
			indices[(size_t)k + 4] = i * n + j + 1;
			indices[(size_t)k + 5] = (i + 1) * n + j + 1;

			k += 6; // next quad
		}
	}
	mesh = std::make_unique<Mesh>();
	mesh->name = "water";

	mesh->p_vertexBuffer = nullptr;
	mesh->p_indexBuffer = rs->createIndexBuffer(&indices[0], sizeof(uint16_t), (UINT)indices.size());

	submesh.indexCount = (UINT)indices.size();
	mesh->drawArgs["grid"] = submesh;

	p_meshes["water"] = std::move(mesh);

	//Render Items
	auto wavesRitem = std::make_unique<RenderItem>();
	DirectX::XMStoreFloat4x4(&wavesRitem->world, DirectX::XMMatrixIdentity());
	wavesRitem->objCBIndex = 0;
	wavesRitem->mesh = p_meshes["water"].get();
	wavesRitem->primitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	wavesRitem->indexCount = wavesRitem->mesh->drawArgs["grid"].indexCount;
	wavesRitem->startIndexLocation = wavesRitem->mesh->drawArgs["grid"].startIndexLocation;
	wavesRitem->baseVertexLocation = wavesRitem->mesh->drawArgs["grid"].baseVertexLocation;

	p_wavesRitem = wavesRitem.get();

	p_ritemLayer[(int)RenderLayer::opaque].push_back(wavesRitem.get());

	auto gridRitem = std::make_unique<RenderItem>();
	DirectX::XMStoreFloat4x4(&gridRitem->world, DirectX::XMMatrixIdentity());
	gridRitem->objCBIndex = 1;
	gridRitem->mesh = p_meshes["land"].get();
	gridRitem->primitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	gridRitem->indexCount = gridRitem->mesh->drawArgs["grid"].indexCount;
	gridRitem->startIndexLocation = gridRitem->mesh->drawArgs["grid"].startIndexLocation;
	gridRitem->baseVertexLocation = gridRitem->mesh->drawArgs["grid"].baseVertexLocation;

	p_ritemLayer[(int)RenderLayer::opaque].push_back(gridRitem.get());

	p_allRitems.push_back(std::move(wavesRitem));
	p_allRitems.push_back(std::move(gridRitem));

	//Frame resources
	for (int i = 0; i < gNumFrameResources; ++i)
	{
		p_frameResources.push_back(std::make_unique<FrameResource>(1, (UINT)p_allRitems.size(), p_waves->vertexCount(), rs));
	}

	p_PSOs["opaque"] = rs->createPipelineState(p_inputLayout, p_VSs["standardVS"], p_PSs["opaquePS"]);

	cmd->closeCmdList();
	cmd->setPSO(p_PSOs["opaque"]);
}

void WavesDemo::onResize()
{
	auto cmd = getGraphicsEngine()->getRenderSystem()->getCommandMgr();

	cmd->resize();

	auto AR = getDisplay()->getAspectRatio();
	DirectX::XMMATRIX P = DirectX::XMMatrixPerspectiveFovLH(0.25f * DirectX::XM_PI, AR, 1.0f, 1000.0f);
	DirectX::XMStoreFloat4x4(&p_proj, P);
}

void WavesDemo::onUpdate(float deltaTime)
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
		float dx = 0.2f * deltaMouse.x;
		float dy = 0.2f * deltaMouse.y;

		p_radius += dx - dy;
		p_radius = max(5.0f, min(p_radius, 150.0f));
	}

	if (GetAsyncKeyState('Y') & 0x8000)
	{
		getGraphicsEngine()->getRenderSystem()->setMSAAState(true);
		p_PSOs["opaque"].reset();
		p_PSOs["opaque"] = getGraphicsEngine()->getRenderSystem()->createPipelineState(p_inputLayout, p_VSs["standardVS"], p_PSs["opaquePS"]);
		//cmdMgr->resetCmdList();
		cmdMgr->setPSO(p_PSOs["opaque"]);
		//getDisplay()->onSize();
	}

	if (GetAsyncKeyState('N') & 0x8000)
	{
		getGraphicsEngine()->getRenderSystem()->setMSAAState(false);
		p_PSOs["opaque"].reset();
		p_PSOs["opaque"] = getGraphicsEngine()->getRenderSystem()->createPipelineState(p_inputLayout, p_VSs["standardVS"], p_PSs["opaquePS"]);
		cmdMgr->setPSO(p_PSOs["opaque"]);
	}

	if (GetAsyncKeyState('1') & 0x8000)
		p_isWireframe = true;
	else
		p_isWireframe = false;

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
			XMStoreFloat4x4(&objConstants.world, world);

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

	DirectX::XMStoreFloat4x4(&p_mainPassCB.view, view);
	DirectX::XMStoreFloat4x4(&p_mainPassCB.invView, invView);
	DirectX::XMStoreFloat4x4(&p_mainPassCB.proj, proj);
	DirectX::XMStoreFloat4x4(&p_mainPassCB.invProj, invProj);
	DirectX::XMStoreFloat4x4(&p_mainPassCB.viewProj, viewProj);
	DirectX::XMStoreFloat4x4(&p_mainPassCB.invViewProj, invViewProj);
	p_mainPassCB.eyePosW = p_eyePos;
	p_mainPassCB.renderTargetSize = DirectX::XMFLOAT2((float)clientWidth, (float)clientHeight);
	p_mainPassCB.invRenderTargetSize = DirectX::XMFLOAT2(1.0f / clientWidth, 1.0f / clientHeight);
	p_mainPassCB.nearZ = 1.0f;
	p_mainPassCB.farZ = 1000.0f;
	p_mainPassCB.totalTime = getTimer()->totalTime();
	p_mainPassCB.deltaTime = deltaTime;

	auto& currPassCB = p_currFrameResource->p_passCB;
	currPassCB->update(0, &p_mainPassCB);

	//update waves
	static float t_base = 0.0f;
	if ((getTimer()->totalTime() - t_base) >= 0.25f)
	{
		t_base += 0.25f;

		int i = MathHelper::rand(4, p_waves->rowCount() - 5);
		int j = MathHelper::rand(4, p_waves->columnCount() - 5);

		float r = MathHelper::randF(0.2f, 0.5f);

		p_waves->disturb(i, j, r);
	}

	p_waves->update(deltaTime);

	auto& currWavesVB = p_currFrameResource->p_wavesVB;
	for (int i = 0; i < p_waves->vertexCount(); ++i)
	{
		Vertex v{};

		v.pos = p_waves->position(i);
		v.color = DirectX::XMFLOAT4(DirectX::Colors::Blue);

		currWavesVB->update(i, &v);
	}

	p_wavesRitem->mesh->p_vertexBuffer = currWavesVB;

	//Drawing
	auto& cmdListAlloc = p_currFrameResource->p_cmdListAlloc;

	cmdMgr->begin(cmdListAlloc.Get());
	cmdMgr->setViewportSize();
	cmdMgr->clearRenderTargetColor(DirectX::Colors::LightSteelBlue);

	//cmdMgr->setDescriptorHeaps();
	cmdMgr->setRootSignature();

	auto& passCB = p_currFrameResource->p_passCB;
	cmdMgr->setCBV(1, passCB);


	auto& objectCB = p_currFrameResource->p_objectCB;
	for (size_t i = 0; i < p_ritemLayer[(int)RenderLayer::opaque].size(); ++i)
	{
		auto ri = p_ritemLayer[(int)RenderLayer::opaque][i];

		cmdMgr->setVertexBuffer(ri->mesh->p_vertexBuffer);
		cmdMgr->setIndexBuffer(ri->mesh->p_indexBuffer);

		cmdMgr->setCBV(0, ri->objCBIndex, objectCB);

		cmdMgr->drawIndexedTriangleList(ri->indexCount, ri->startIndexLocation, ri->baseVertexLocation);
	}


	cmdMgr->finish();

	scptr->present(true);

	cmdMgr->signal(p_currFrameResource->p_fence);
}

float WavesDemo::getHillsHeight(float x, float z) const
{
	return 0.3f * (z * sinf(0.1f * x) + x * cosf(0.1f * z));
}
