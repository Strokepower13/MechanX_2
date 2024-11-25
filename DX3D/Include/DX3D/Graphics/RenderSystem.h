#pragma once
#include <DX3D/Prerequisites.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <wrl.h>
#include <vector>

#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

//extern "C" { __declspec(dllexport) extern const UINT D3D12SDKVersion = 614; }
//
//extern "C" { __declspec(dllexport) extern const char* D3D12SDKPath = u8".\\D3D12\\"; }

class RenderSystem
{
public:
	//Initialize the GraphicsEngine and DirectX 12 Device
	RenderSystem();
	//Releaze all the resources loaded
	~RenderSystem();

	SwapChainPtr createSwapChain(HWND hwnd, UINT width, UINT height);
	//SwapChainPtr createSwapChain4xMsaa(HWND hwnd, UINT width, UINT height);
	CommandManagerPtr getCommandMgr();

	VertexBufferPtr createVertexBuffer(const void* data, UINT sizeVertex, UINT sizeList);
	IndexBufferPtr createIndexBuffer(const void* data, UINT sizeVertex, UINT sizeList);
	ConstantBufferPtr createConstantBuffer(UINT sizeData, UINT elementCount);
	PipelineStatePtr createPipelineState(const InputLayoutPtr& inputLayout, const VertexShaderPtr& vs, const PixelShaderPtr& ps);
	VertexShaderPtr createVertexShader(const wchar_t* fullPath);
	//GeometryShaderPtr createGeometryShader(const wchar_t* fullPath, const char* entryPoint);
	PixelShaderPtr createPixelShader(const wchar_t* fullPath);
	//TexturePtr createTexture(const wchar_t* fullPath);
	//Texture2DArrayPtr createTexture2DArray(std::vector<std::wstring>& filenames);

	//VertexBufferPtr createVertexBufferDynamic(UINT sizeVertex, UINT sizeList);

	//void clearState();

	//RasterizerStatePtr createRasterizerState(const FillMode& fillMode, const CullMode& cullMode, bool frontCounterClockwise = false);
	//BlendStatePtr createBlendState(const BlendMode& blendMode);
	//DepthStencilStatePtr createDepthStencilState(const DepthStencilMode& dsMode);

	MSAAResourcesPtr createMSAAResources(UINT width, UINT height);
	void setMSAAState(bool state);
	bool getMSAAState();

private:
	void logAdapters();
	void logAdapterOutputs(IDXGIAdapter* adapter);
	void logOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format);

private:
	Microsoft::WRL::ComPtr<IDXGIFactory4> p_dxgiFactory;
	Microsoft::WRL::ComPtr<ID3D12Device> p_d3dDevice;

	bool p_4xMsaaState = false;
	UINT p_4xMsaaQuality = 0;

	DXGI_FORMAT p_backBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGI_FORMAT p_depthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	static const int p_swapChainBufferCount = 2;

	CommandManagerPtr p_commandMgr;
	DescriptorHeapPtr p_descriptorHeap;
	RootSignaturePtr p_rootSignature;

	SwapChainPtr p_swapChain = nullptr;
	MSAAResourcesPtr p_msaaRes = nullptr;

	friend class SwapChain;
	friend class CommandManager;
	friend class DescriptorHeap;
	friend class RootSignature;
	friend class PipelineState;
	friend class VertexBuffer;
	friend class IndexBuffer;
	friend class ConstantBuffer;
	friend class VertexShader;
	friend class GeometryShader;
	friend class PixelShader;
	friend class Texture;
	friend class Texture2DArray;
	friend class RasterizerState;
	friend class BlendState;
	friend class DepthStencilState;
	friend class MSAAResources;
};

