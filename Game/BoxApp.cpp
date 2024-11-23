#include "BoxApp.h"

struct Vertex
{
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT4 color;
};

struct ObjectConstants
{
	DirectX::XMFLOAT4X4 worldViewProj;
};

BoxApp::BoxApp(HINSTANCE hInstance) : Game(hInstance)
{
	DirectX::XMMATRIX I = DirectX::XMMatrixIdentity();
	DirectX::XMStoreFloat4x4(&p_world, I);
	DirectX::XMStoreFloat4x4(&p_view, I);
	DirectX::XMStoreFloat4x4(&p_proj, I);
}

BoxApp::~BoxApp()
{
}

void BoxApp::onCreate()
{
	auto commMgr = getGraphicsEngine()->getRenderSystem()->getCommandMgr();

	commMgr->resetCmdList();

	p_inputLayout = std::make_shared<InputLayout>(InputLayoutType::PosColor);

	auto rs = getGraphicsEngine()->getRenderSystem();

	Vertex vertices[8] = {
		Vertex({ DirectX::XMFLOAT3(-1.0f, -1.0f, -1.0f), DirectX::XMFLOAT4(DirectX::Colors::White) }),
		Vertex({ DirectX::XMFLOAT3(-1.0f, +1.0f, -1.0f), DirectX::XMFLOAT4(DirectX::Colors::Black) }),
		Vertex({ DirectX::XMFLOAT3(+1.0f, +1.0f, -1.0f), DirectX::XMFLOAT4(DirectX::Colors::Red) }),
		Vertex({ DirectX::XMFLOAT3(+1.0f, -1.0f, -1.0f), DirectX::XMFLOAT4(DirectX::Colors::Green) }),
		Vertex({ DirectX::XMFLOAT3(-1.0f, -1.0f, +1.0f), DirectX::XMFLOAT4(DirectX::Colors::Blue) }),
		Vertex({ DirectX::XMFLOAT3(-1.0f, +1.0f, +1.0f), DirectX::XMFLOAT4(DirectX::Colors::Yellow) }),
		Vertex({ DirectX::XMFLOAT3(+1.0f, +1.0f, +1.0f), DirectX::XMFLOAT4(DirectX::Colors::Cyan) }),
		Vertex({ DirectX::XMFLOAT3(+1.0f, -1.0f, +1.0f), DirectX::XMFLOAT4(DirectX::Colors::Magenta) })
	};

	uint16_t indices[36] = {
		// front face
		0, 1, 2,
		0, 2, 3,

		// back face
		4, 6, 5,
		4, 7, 6,

		// left face
		4, 5, 1,
		4, 1, 0,

		// right face
		3, 2, 6,
		3, 6, 7,

		// top face
		1, 5, 6,
		1, 6, 2,

		// bottom face
		4, 0, 3,
		4, 3, 7
	};

	p_vb = rs->createVertexBuffer(vertices, sizeof(Vertex), ARRAYSIZE(vertices));
	p_ib = rs->createIndexBuffer(indices, sizeof(uint16_t), ARRAYSIZE(indices));

	p_vs = rs->createVertexShader(L"Assets/Shaders/ColorVS.cso");
	p_ps = rs->createPixelShader(L"Assets/Shaders/ColorPS.cso");

	ObjectConstants cb{};

	p_cb = rs->createConstantBuffer(sizeof(ObjectConstants), 1);

	p_pso = rs->createPipelineState(p_inputLayout, p_vs, p_ps);

	commMgr->closeCmdList();

	commMgr->setPSO(p_pso);
}

void BoxApp::onUpdate(float deltaTime)
{
	auto scptr = getDisplay()->getSwapChain();
	auto cmdMgr = getGraphicsEngine()->getRenderSystem()->getCommandMgr();
	auto AR = getDisplay()->getAspectRatio();

	int width = getDisplay()->getClientWidth();
	int height = getDisplay()->getClientHeight();
	
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

	ObjectConstants cb{};
	float x = p_radius * sinf(p_phi) * cosf(p_theta);
	float z = p_radius * sinf(p_phi) * sinf(p_theta);
	float y = p_radius * cosf(p_phi);

	DirectX::XMVECTOR pos = DirectX::XMVectorSet(x, y, z, 1.0f);
	DirectX::XMVECTOR target = DirectX::XMVectorZero();
	DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	DirectX::XMMATRIX V = DirectX::XMMatrixLookAtLH(pos, target, up);
	DirectX::XMStoreFloat4x4(&p_view, V);

	DirectX::XMMATRIX P = DirectX::XMMatrixPerspectiveFovLH(0.25f * DirectX::XM_PI, float(width) / height, 1.0f, 1000.0f);
	DirectX::XMStoreFloat4x4(&p_proj, P);

	DirectX::XMStoreFloat4x4(&p_world, DirectX::XMMatrixIdentity());

	DirectX::XMMATRIX world = DirectX::XMLoadFloat4x4(&p_world);
	DirectX::XMMATRIX worlViewProj = world * V * P;

	DirectX::XMStoreFloat4x4(&cb.worldViewProj, worlViewProj);

	p_cb->update(0, &cb);

	//Begin drawing
	cmdMgr->begin(scptr);
	cmdMgr->setViewportSize(scptr);
	cmdMgr->clearRenderTargetColor(scptr, DirectX::Colors::LightSteelBlue);

	cmdMgr->setDescriptorHeaps();
	cmdMgr->setRootSignature();

	cmdMgr->setVertexBuffer(p_vb);
	cmdMgr->setIndexBuffer(p_ib);
	cmdMgr->setDescriptorTable();
	cmdMgr->drawIndexedTriangleList(36, 0, 0);
	cmdMgr->finish(scptr);

	scptr->present(true);
}
